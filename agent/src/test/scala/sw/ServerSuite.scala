package sw

import cats.effect.*
import cats.syntax.all.*
import org.http4s.*
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import org.http4s.client.UnexpectedStatus

import sw.domain.ConversionRate
import sw.domain.ConversionRate.given
import sw.service.FreeCurrencyApi

class ServerSuite extends munit.CatsEffectSuite:
  test("Server responds to pings") {
    val request = Request[IO](Method.GET, uri"/ping")

    val mockService = new FreeCurrencyApi:
      def latest(base: String, targets: List[String]): IO[Map[String, Double]] =
        IO(Map("USD" -> 23.5))

    Server.routes(mockService).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[String].assertEquals(Server.message)
    }
  }

  test("Server converts currencies") {
    val request = Request[IO](Method.GET, uri"/convert/USD/PLN")

    val mockService = new FreeCurrencyApi:
      def latest(base: String, targets: List[String]): IO[Map[String, Double]] =
        IO(Map("PLN" -> 23.5))

    Server.routes(mockService).orNotFound.run(request).flatMap { response =>
      val expected = """{"sourceCurrency":"USD","destinationCurrency":"PLN","rate":23.5}"""
      assertEquals(response.status, Status.Ok)
      response.as[String].assertEquals(expected)
    }
  }

  test("Server handles unprocessable currencies") {
    val url = uri"/convert/USD/PLN"
    val request = Request[IO](Method.GET, url)

    val mockService = new FreeCurrencyApi:
      def latest(base: String, targets: List[String]): IO[Map[String, Double]] =
        IO.raiseError(UnexpectedStatus(Status.UnprocessableEntity, Method.GET, url))

    Server.routes(mockService).orNotFound.run(request).map { response =>
      assertEquals(response.status, Status.UnprocessableEntity)
    }
  }

  test("Server handles internal errors") {
    val url = uri"/convert/USD/PLN"
    val request = Request[IO](Method.GET, url)

    val mockService = new FreeCurrencyApi:
      def latest(base: String, targets: List[String]): IO[Map[String, Double]] =
        IO.raiseError(BadValue("Bad value, I dunno."))

    Server.routes(mockService).orNotFound.run(request).map { response =>
      assertEquals(response.status, Status.InternalServerError)
    }
  }
