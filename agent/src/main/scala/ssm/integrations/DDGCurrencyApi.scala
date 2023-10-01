package ssm.integrations

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


trait DDGCurrencyApi:
  def latest(base: String, targets: String): IO[Double]

object DDGCurrencyApi:
  private case class Conversion(quotecurrency: String, mid: Double)
  private case class CurrencyApiResult(from: String, to: List[Conversion])

  case class BadValue(message: String) extends Throwable(message)

  val CouldNotExtractResponse = BadValue("Could not extract the response data from DDG spice API.")
  val CouldNotExtractConversionRate = BadValue("Could not extract the conversion rate data from DDG spice API response.")

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
        parsed <- IO.fromEither(decode[CurrencyApiResult](stripped))
        first <- IO.fromOption(parsed.to.headOption)(DDGCurrencyApi.CouldNotExtractConversionRate)
      yield first.mid
