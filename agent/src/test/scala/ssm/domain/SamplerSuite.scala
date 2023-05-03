package ssm.domain

import cats.effect.*
import cats.implicits.*
import munit.*
import fs2.Stream

import scala.concurrent.duration.*
import fs2.Chunk

class SamplerSuite extends CatsEffectSuite:
  def mockSampler(samples: List[Double]): Sampler.SamplerImpl =
    new Sampler.SamplerImpl("test", IO.never):
        override def sampleStream(samplingInterval: FiniteDuration): Stream[IO, Double] =
          Stream.emits(samples)

  test("Should respond with empty stats, errors and status") {
    val sampler = mockSampler(List.empty)

    for
      status <- sampler.status
      errors <- sampler.errors
      stats <- sampler.stats
    yield {
      assertEquals(status, Sampler.Status.Init)
      assertEquals(errors, Sampler.Errors(0, ""))
      assertEquals(stats, Sampler.Stats(0, 0, 0, 0, 0, 0, 0))
    }
  }

  test("Should respond with some stats when data is available") {
    val sampler = mockSampler(List(1.0, 2.0, 3.0))

    for
      _ <- sampler.samplerStream(10.millis, 10).compile.toList
      status <- sampler.status
      errors <- sampler.errors
      stats <- sampler.stats
    yield {
      assertEquals(status, Sampler.Status.Ok)
      assertEquals(errors, Sampler.Errors(0, ""))
      assertEquals(stats, Sampler.Stats(3.0, 2.0, 2.0, 3, 1.0, 3.0, 3))
    }
  }

  test("Should store an error when it happens") {
    val sampler = new Sampler.SamplerImpl("test", IO.raiseError(new Throwable("Oof")))

    for
      _ <- sampler.samplerStream(100.millis, 10).interruptAfter(20.millis).compile.toList
      status <- sampler.status
      errors <- sampler.errors
      stats <- sampler.stats
    yield {
      assertEquals(status, Sampler.Status.Error)
      assertEquals(errors, Sampler.Errors(1, "Oof"))
      assertEquals(stats, Sampler.Stats(0, 0, 0, 0, 0, 0, 0))
    }
  }

  test("Should recover from an error eventually") {
    val samples = List(1.0, 23.5, 2.0, 23.5, 3.0)
    var i = 0 // FIXME Oof
    val gen = IO {
      i = i + 1
      if i > samples.length
      then 0.0
      else if samples(i-1) > 23
      then throw new Throwable("Oops")
      else samples(i-1)
    }
    val sampler = new Sampler.SamplerImpl("test", gen)

    for
      _ <- sampler.samplerStream(5.millis, 10).take(3).compile.toList
      status <- sampler.status
      errors <- sampler.errors
      stats <- sampler.stats
    yield {
      assertEquals(status, Sampler.Status.Ok)
      assertEquals(errors, Sampler.Errors(2, "Oops"))
      assertEquals(stats, Sampler.Stats(3.0, 2.0, 2.0, 3, 1.0, 3.0, 3))
    }
  }
