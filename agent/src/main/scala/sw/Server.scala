package sw

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

import sw.domain.ConversionRate
import sw.domain.ConversionRate.given

import sw.service.FreeCurrencyApi

case class BadValue(message: String) extends Throwable(message)

object Server:
  val message = "Hello World"

  given EntityDecoder[IO, ValidatedNec[String, ConversionRate]] = accumulatingJsonOf[IO, ValidatedNec[String, ConversionRate]]

  def routes(service: FreeCurrencyApi) = HttpRoutes.of[IO] {

    case GET -> Root / "ping" =>
      Ok(message)

    case GET -> Root / "convert" / source / destination =>
      val response = for {
        latest <- service.latest(source, List(destination))
        value <- IO.fromOption(latest.get(destination))(BadValue(s"Did not receive the expected value for: $destination"))
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
