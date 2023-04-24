package ssm.domain

import cats.effect.*
import cats.implicits.*
import io.circe.Json
import ssm.model.generated.*
import ssm.service.*

import scala.concurrent.duration.*

trait System:
  def observer: Resource[IO, Unit]
  def readings: IO[List[Reading]]

object System:
  def assemble(config: Config, mdns: MDNS, currencyApi: DDGCurrencyApi): System =
    new SystemImpl(config, mdns, currencyApi)

  private class SystemImpl(config: Config, mdns: MDNS, currencyApi: DDGCurrencyApi) extends System:

    private val system: List[ObservableReading] = {

      val displayConfig = (color: String) => Map(
        "type" -> Json.fromString("gauge"),
        "color" -> Json.fromString(color)
      )

      val usd = ObservableReading("ddg", "currency", "USD", currencyApi.latest("USD", "PLN"), "PLN", 4.0, 5.0, displayConfig("green"))
      val eur = ObservableReading("ddg", "currency", "EUR", currencyApi.latest("EUR", "PLN"), "PLN", 4.0, 5.0, displayConfig("blue"))
      val gbp = ObservableReading("ddg", "currency", "GBP", currencyApi.latest("GBP", "PLN"), "PLN", 4.0, 6.0, displayConfig("purple"))
      val xag = ObservableReading("ddg", "currency", "XAG", currencyApi.latest("XAG", "PLN"), "PLN", 50.0, 200.0, displayConfig("silver"))
      val xau = ObservableReading("ddg", "currency", "XAU", currencyApi.latest("XAU", "PLN"), "PLN", 6000.0, 10000.0, displayConfig("gold"))
      val btc = ObservableReading("ddg", "currency", "BTC", currencyApi.latest("XBT", "PLN").map(_ / 1000.0), "kPLN", 100.0, 200.0, displayConfig("gray"))

      List(usd, eur, gbp, xag, xau)
    }

    override def observer: Resource[IO, Unit] =
      system.map(_.observer(1.hour, 3)).sequence.map(_ => ())

    override def readings: IO[List[Reading]] =
      system.map(_.reading).sequence
