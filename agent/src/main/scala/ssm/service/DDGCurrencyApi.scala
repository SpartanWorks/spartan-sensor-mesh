package ssm.service

import scala.util.{Try, Failure}

import cats.effect.*
import org.http4s.*
import org.http4s.circe.*
import org.http4s.client.*
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import io.circe.*
import io.circe.generic.auto.*
import io.circe.parser.*
import org.http4s.client.dsl.Http4sClientDsl

case class DDGConversion(`from-currency-symbol`: String, `to-currency-symbol`: String, `converted-amount`: String)
case class DDGCurrencyApiResult(conversion: DDGConversion)

case class DDGBadValue(message: String) extends Throwable(message)

trait DDGCurrencyApi:
  def latest(base: String, targets: String): IO[Double]

object DDGCurrencyApi:
  val CouldNotExtractResponse = DDGBadValue("Could not extract the response data from DDG spice API.")

  def apply(client: Client[IO]): DDGCurrencyApi =
    new DDGCurrencyApiImpl(client)

  private class DDGCurrencyApiImpl(client: Client[IO]) extends DDGCurrencyApi with Http4sClientDsl[IO]:

    private val ApiUri = "https://duckduckgo.com/js/spice/currency"
    private val Spice = """ddg_spice_currency\((.+)\);""".r

    def latest(base: String, target: String): IO[Double] =
      for
        uri <- IO.fromEither(Uri.fromString(s"$ApiUri/1/$base/$target"))
        response <- client.expect[String](GET(uri))
        flat = response.replace("\n", "")
        matched <- IO.fromOption(Spice.findFirstMatchIn(flat))(DDGCurrencyApi.CouldNotExtractResponse)
        stripped <- IO.fromTry(Try(matched.group(1).strip).recoverWith(_ => Failure(DDGCurrencyApi.CouldNotExtractResponse)))
        parsed <- IO.fromEither(decode[DDGCurrencyApiResult](stripped))
        value <- IO.fromTry(Try(parsed.conversion.`converted-amount`.toDouble))
      yield value
