package ssm.domain

import cats.effect.*
import io.circe.Json
import ssm.model.generated.*
import ssm.service.Sampler

import scala.concurrent.duration.FiniteDuration

trait ObservableReading:
  def observer(samplingInterval: FiniteDuration, windowSize: Int): Resource[IO, Unit]
  def reading: IO[Reading]

object ObservableReading:
  def apply(model: String, `type`: String, name: String, sample: IO[Double], unit: String, min: Double, max: Double, displayConfig: Map[String, Json]): ObservableReading =
    new ObservableReadingImpl(model, `type`, name, unit, min, max, displayConfig, Sampler(s"${`type`}/$model/$name", sample))

  class ObservableReadingImpl(model: String, `type`: String, name: String, unit: String, min: Double, max: Double, displayConfig: Map[String, Json], sampler: Sampler) extends ObservableReading:
    override def observer(samplingInterval: FiniteDuration, windowSize: Int): Resource[IO, Unit] =
      sampler.sampler(samplingInterval, windowSize)

    override def reading: IO[Reading] =
      for
        status <- sampler.status
        errors <- sampler.errors
        stats <- sampler.stats
      yield Reading(
        model,
        `type`,
        name,
        status.toString.toLowerCase,
        errors.count,
        errors.lastMessage,
        stats.total,
        ReadingValue(
          stats.latest,
          unit,
          ReadingValueStats(stats.mean, stats.variance, stats.count, Some(stats.min), Some(stats.max)),
          ReadingValueRange(min, max)
        ),
        displayConfig
      )
