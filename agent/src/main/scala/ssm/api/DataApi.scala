package ssm.api

import cats.data.*
import cats.effect.*
import cats.implicits.*
import fs2.Stream
import io.circe.syntax.*
import io.circe.disjunctionCodecs.*
import org.http4s.*
import org.http4s.circe.*
import org.http4s.circe.CirceEntityEncoder.circeEntityEncoder
import org.http4s.client.UnexpectedStatus
import org.http4s.dsl.io.*
import org.http4s.implicits.*

import sw.generated.model.*
import sw.generated.model.Data.given

import ssm.service.Sampler

import scala.concurrent.duration.*

object DataApi:
  def routes(name: String, model: String, group: String, samplers: List[Sampler]) = HttpRoutes.of[IO] {

    case GET -> Root =>
      samplers.map { s =>
        for
          status <- s.status
          errors <- s.errors
          stats <- s.stats
        yield Reading(
          "currency",
          "currency",
          "USD",
          status.toString.toLowerCase,
          errors.count,
          errors.lastMessage,
          stats.total,
          ReadingValue(
            stats.latest,
            "PLN",
            ReadingValueStats(stats.mean, stats.variance, stats.count, Some(stats.min), Some(stats.max)),
            ReadingValueRange(4.0, 4.5)
          ),
          WidgetConfig("gauge")
        )
      }.sequence.flatMap { readings =>
        Ok(Data(
          model,
          group,
          name,
          readings
        ))
      }
  }
