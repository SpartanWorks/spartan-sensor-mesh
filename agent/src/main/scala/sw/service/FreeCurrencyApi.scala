package sw.service

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

case class FreeCurrencyApiResult(data: Map[String, Double])

trait FreeCurrencyApi:
  def latest(base: String, targets: List[String]): IO[Map[String, Double]]

object FreeCurrencyApi:
  def apply(client: Client[IO], apiKey: String): FreeCurrencyApi =
    new FreeCurrencyApiImpl(client, apiKey)

class FreeCurrencyApiImpl(client: Client[IO], apiKey: String) extends FreeCurrencyApi with Http4sClientDsl[IO]:

  val apiUri = uri"https://api.freecurrencyapi.com/v1/latest"

  def latest(base: String, targets: List[String]): IO[Map[String, Double]] =
    val targetString = targets.mkString(",")
    val targetUri = apiUri
      .withQueryParam("base_currency", base)
      .withQueryParam("currencies", targetString)
      .withQueryParam("apikey", apiKey)

    for
      request <- IO(GET(targetUri))
      response <- client.expect[String](request)
      parsedResponse <- IO.fromEither(decode[FreeCurrencyApiResult](response))
    yield parsedResponse.data
