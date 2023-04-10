package ssm.service

import cats.effect.*
import io.circe.*
import io.circe.syntax.*
import io.circe.generic.auto.*
import fs2.Stream
import org.http4s.*
import org.http4s.client.*
import org.http4s.client.dsl.io.*
import org.http4s.circe.CirceEntityEncoder.*
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import org.http4s.server.*
import org.http4s.server.blaze.*
import org.http4s.headers.*
import org.http4s.syntax.all.*
import munit.*

class DDGCurrencyApiSuite extends CatsEffectSuite:
  val mockedClient = Client.fromHttpApp[IO](HttpRoutes.of[IO] {
    case GET -> Root / "js" / "spice" / "currency" / "1" / "USD" / target =>
      val resp = """
           | ddg_spice_currency(
           |   {
           |     "headers":{
           |       "encoding": "utf-8",
           |       "language": "en",
           |       "legal-warning": "Use of this service is subject to the terms of use at http://www.xe.com/legal/",
           |       "help-notice": "For help using this service, please see: http://www.xe.com/tmi/help.php",
           |       "utc-timestamp": "2023-04-10 07:41:43",
           |       "status":"0",
           |       "output-format":"json",
           |       "description": ""
           |     },
           |     "conversion":{
           |       "rate-utc-timestamp": "2023-04-09 16:00 GMT",
           |       "rate-frequency": "daily rates",
           |       "from-amount": "1",
           |       "from-currency-symbol": "XAU",
           |       "from-currency-name": "Gold Ounces",
           |       "converted-amount": "8599.03",
           |       "to-currency-symbol": "PLN",
           |       "to-currency-name": "Poland Zlotych",
           |       "conversion-rate": "1 XAU = 8599.03 PLN",
           |       "conversion-inverse": "1 PLN = 0.000116 XAU"
           |     },
           |     "topConversions":[
           |       {
           |         "rate-utc-timestamp": "2023-04-09 16:00 GMT",
           |         "from-amount": "1",
           |         "from-currency-symbol": "XAU",
           |         "from-currency-name": "Gold Ounces",
           |         "converted-amount": "2007.77",
           |         "to-currency-symbol": "USD",
           |         "to-currency-name": "United States Dollars",
           |         "conversion-rate": "1 XAU = 2007.77 USD",
           |         "conversion-inverse": "1 USD = 0.000498 XAU"
           |       }
           |     ]
           |   }
           | );
           |""".stripMargin
      IO(Response(body = Stream.emits(resp.getBytes))) // NOTE This is the raw data as-is.
    case GET -> Root / "js" / "spice" / "currency" / "1" / "BAD" / target =>
      Ok("Not the right response format.")
  }.orNotFound)

  val api = DDGCurrencyApi(mockedClient)

  test("Correctly recognizes DDG spice call") {
    api.latest("BAD", "PLN").map { result =>
      assert(false)
    }.handleError { error =>
      assertEquals(error, DDGCurrencyApi.CouldNotExtractResponse)
    }
  }

  test("Latest exchange rates are returned successfully") {
    api.latest("USD", "PLN").map { result =>
      assert(result > 0)
    }
  }
