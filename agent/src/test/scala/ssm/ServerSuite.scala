package ssm

import cats.effect.*
import cats.syntax.all.*
import org.http4s.*
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import org.http4s.client.UnexpectedStatus

import ssm.domain.ConversionRate
import ssm.domain.ConversionRate.given
import ssm.service.{DDGCurrencyApi, DDGBadValue}

class ServerSuite extends munit.CatsEffectSuite:
  test("Server responds to pings") {
    val request = Request[IO](Method.GET, uri"/ping")

    val mockService = new DDGCurrencyApi:
      def latest(base: String, target: String): IO[Double] =
        IO(23.5)

    Server.routes(mockService).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[String].assertEquals(Server.message)
    }
  }

  test("Server converts currencies") {
    val request = Request[IO](Method.GET, uri"/convert/USD/PLN")

    val mockService = new DDGCurrencyApi:
      def latest(base: String, target: String): IO[Double] =
        IO(23.5)

    Server.routes(mockService).orNotFound.run(request).flatMap { response =>
      val expected = """{"sourceCurrency":"USD","destinationCurrency":"PLN","rate":23.5}"""
      assertEquals(response.status, Status.Ok)
      response.as[String].assertEquals(expected)
    }
  }

  test("Server handles unprocessable currencies") {
    val url = uri"/convert/USD/PLN"
    val request = Request[IO](Method.GET, url)

    val mockService = new DDGCurrencyApi:
      def latest(base: String, target: String): IO[Double] =
        IO.raiseError(UnexpectedStatus(Status.UnprocessableEntity, Method.GET, url))

    Server.routes(mockService).orNotFound.run(request).map { response =>
      assertEquals(response.status, Status.UnprocessableEntity)
    }
  }

  test("Server handles internal errors") {
    val url = uri"/convert/USD/PLN"
    val request = Request[IO](Method.GET, url)

    val mockService = new DDGCurrencyApi:
      def latest(base: String, target: String): IO[Double] =
        IO.raiseError(DDGBadValue("Bad value, I dunno."))

    Server.routes(mockService).orNotFound.run(request).map { response =>
      assertEquals(response.status, Status.InternalServerError)
    }
  }
