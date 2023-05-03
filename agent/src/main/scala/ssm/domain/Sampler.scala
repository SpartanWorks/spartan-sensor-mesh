package ssm.domain

import cats.effect.*
import fs2.Stream
import ssm.domain.ReadingOps.withStats
import ssm.Log.*

import scala.concurrent.duration.FiniteDuration

trait Sampler:
  def sampler(samplingInterval: FiniteDuration, windowSize: Int): Resource[IO, Unit]
  def status: IO[Sampler.Status]
  def stats: IO[Sampler.Stats]
  def errors: IO[Sampler.Errors]

object Sampler:
  type Stats = ReadingOps.Stats
  val Stats = ReadingOps.Stats

  enum Status:
    case Init, Ok, Error

  case class Errors(count: Int, lastMessage: String)

  def apply(name: String, sample: IO[Double]): Sampler =
    new SamplerImpl(name, sample)

  private[domain] class SamplerImpl(name: String, sample: IO[Double]) extends Sampler:
    private val log = getLogger
    private val statusRef = Ref.unsafe[IO, Status](Status.Init)
    private val statsRef = Ref.unsafe[IO, Stats](Stats(0, 0, 0, 0, 0, 0, 0))
    private val errorsRef = Ref.unsafe[IO, Errors](Errors(0, ""))

    // NOTE These are extracted for mocking in tests.
    protected[domain] def sampleStream(samplingInterval: FiniteDuration): Stream[IO, Double] =
      Stream.repeatEval(
        sample.map { s => Some(s) }.handleErrorWith { e =>
          for
            curr <- errorsRef.get
            _ <- errorsRef.set(Errors(curr.count + 1, e.getMessage))
            _ <- statusRef.set(Status.Error)
            _ <- log.error(s"Could not sample a value: ${e.getMessage}")
          yield None
      }).meteredStartImmediately(samplingInterval)
        .unNone

    protected[domain] def samplerStream(samplingInterval: FiniteDuration, windowSize: Int): Stream[IO, Unit] =
      for
        _ <- log.info(s"Sampling $name every $samplingInterval.").stream
        stats <- sampleStream(samplingInterval).withStats(windowSize)
        _ <- Stream.eval(statsRef.set(stats))
        _ <- Stream.eval(statusRef.set(Status.Ok))
      yield ()

    def sampler(samplingInterval: FiniteDuration, windowSize: Int): Resource[IO, Unit] =
      samplerStream(samplingInterval, windowSize).compile.resource.drain

    def status: IO[Status] =
      statusRef.get

    def stats: IO[Stats] =
      statsRef.get

    def errors: IO[Errors] =
      errorsRef.get

