package ssm.integrations

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
           |     "terms":"",
           |     "privacy":"",
           |     "from":"USD",
           |     "amount":1.0,
           |     "timestamp":"2023-10-01T20:08:00Z",
           |     "to": [
           |       {
           |         "quotecurrency":"PLN",
           |         "mid":4.3713939758
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
