package ssm.domain

import cats.effect.*
import fs2.Stream

object ReadingOps:

  case class Stats(latest: Double, mean: Double, variance: Double, count: Int, min: Double, max: Double, total: Int)

  private case class State(samples: List[Double], min: Double, max: Double, total: Int)

  extension (stream: Stream[IO, Double])
    def withStats(samples: Int = 50): Stream[IO, Stats] =
      stream.mapAccumulate(State(List.empty, Double.MaxValue, Double.MinValue, 0)) { case (state, latest) =>
        val values = (latest :: state.samples).take(samples)
        val sum = values.reduce(_ + _)
        val mean = sum / values.length
        val variance = values.map { v =>
          val diff = v - mean
          diff * diff
        }.reduce(_ + _)
        val newMin = scala.math.min(state.min, latest)
        val newMax = scala.math.max(state.max, latest)
        val newTotal = state.total + 1

        (State(values, newMin, newMax, newTotal), Stats(latest, mean, variance, values.length, newMin, newMax, newTotal))
      }.map { case (_, result) => result }
