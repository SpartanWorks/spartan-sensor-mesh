package ssm.api

import cats.data.*
import cats.effect.*
import fs2.Stream
import io.circe.syntax.*
import io.circe.disjunctionCodecs.*
import org.http4s.*
import org.http4s.circe.*
import org.http4s.circe.CirceEntityEncoder.circeEntityEncoder
import org.http4s.client.UnexpectedStatus
import org.http4s.dsl.io.*
import org.http4s.implicits.*

import ssm.model.generated.*
import ssm.model.generated.Data.given
import ssm.service.Node

import scala.concurrent.duration.*

object DataApi:
  def routes(node: Node) = HttpRoutes.of[IO] {

    case GET -> Root =>
      for
        data <- node.data
        resp <- Ok(data)
      yield resp

  }
