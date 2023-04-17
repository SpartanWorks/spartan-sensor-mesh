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

import ssm.domain.ObservableReading

import scala.concurrent.duration.*

object DataApi:
  def routes(name: String, model: String, group: String, readings: List[ObservableReading]) = HttpRoutes.of[IO] {

    case GET -> Root =>
      for
        rs <- readings.map(_.reading).sequence
        data = Data(model, group, name, rs)
        resp <- Ok(data)
      yield resp

  }
