package ssm

import cats.data.*
import cats.effect.*
import cats.implicits.*
import io.circe.syntax.*
import io.circe.disjunctionCodecs.*
import org.http4s.*
import org.http4s.circe.*
import org.http4s.client.UnexpectedStatus
import org.http4s.dsl.io.*
import org.http4s.implicits.*

import ssm.domain.ConversionRate
import ssm.domain.ConversionRate.given

import ssm.service.DDGCurrencyApi

object Server:
  val message = "Hello World"

  given EntityDecoder[IO, ValidatedNec[String, ConversionRate]] = accumulatingJsonOf[IO, ValidatedNec[String, ConversionRate]]

  def routes(service: DDGCurrencyApi) = HttpRoutes.of[IO] {

    case GET -> Root / "ping" =>
      Ok(message)

    case GET -> Root / "convert" / source / destination =>
      val response = for {
        value <- service.latest(source, destination)
        result <- ConversionRate
          .create(source, destination, value)
          .fold(e => BadRequest(e.foldLeft("")(_ ++ _)), c => Ok(c.asJson))
      } yield result

      response.handleErrorWith {
        // NOTE Fetching an unsupported currency.
        case UnexpectedStatus(Status.UnprocessableEntity, _, _) => UnprocessableEntity()
        // NOTE So that we get better error messages.
        case other => InternalServerError(other.getMessage())
      }

    case unknown =>
      NotFound()
  }
