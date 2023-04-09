package sw.service

import cats.effect.*
import io.circe.*
import io.circe.syntax.*
import io.circe.generic.auto.*
import org.http4s.*
import org.http4s.client.*
import org.http4s.client.dsl.io.*
import org.http4s.circe.CirceEntityEncoder.*
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import org.http4s.server.*
import org.http4s.server.blaze.*
import org.http4s.syntax.all.*

import munit.*

class FreeCurrencyApiSuite extends CatsEffectSuite:
  val baseCurrency = "PLN"
  val targetCurrencies = List("EUR", "USD", "XAU", "PLN")

  val mockedClient = Client.fromHttpApp[IO](HttpRoutes.of[IO] {
    case GET -> Root / "v1" / "latest" =>
      Ok(FreeCurrencyApiResult(Map(
        "EUR" -> 0.85,
        "USD" -> 0.72,
        "XAU" -> 109.47,
        "PLN" -> 1.0
      )))
  }.orNotFound)

  val api = FreeCurrencyApi(mockedClient, "apiKey")

  test("Latest exchange rates are returned successfully") {
    api.latest(baseCurrency, targetCurrencies).map { result =>
      targetCurrencies.map { currency =>
        assert(result.contains(currency))
      }
    }
  }

  test("Exchange rates for base currency are correct") {
    api.latest(baseCurrency, targetCurrencies).map { result =>
      assert(result("EUR") > 0)
      assert(result("USD") > 0)
      assert(result("XAU") > 0)
      assert(result(baseCurrency) == 1.0)
    }
  }
