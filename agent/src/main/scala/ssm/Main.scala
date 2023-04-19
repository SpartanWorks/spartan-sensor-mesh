package ssm

import cats.effect.*
import io.circe.Json
import org.http4s.implicits.*
import org.http4s.server.{ Server, Router }
import org.http4s.server.middleware.Logger
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder
import org.http4s.server.middleware.*

import ssm.api.*
import ssm.domain.*
import ssm.Log.*
import ssm.service.*

import scala.concurrent.duration.*

object Main extends ResourceApp.Forever:
  private val log = getLogger

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      _ <- log.info("Loading configuration...").background
      config <- Config.load()

      _ <- log.info("Booting services...").background
      client <- BlazeClientBuilder[IO].resource
      mdns = MDNS(config.mdns.serviceName, config.mdns.serviceType)
      currency = DDGCurrencyApi(client)

      displayConfig = (color: String) => Map(
        "type" -> Json.fromString("gauge"),
        "color" -> Json.fromString(color)
      )

      usd = ObservableReading("ddg", "currency", "USD", currency.latest("USD", "PLN"), "PLN", 4.0, 5.0, displayConfig("green"))
      eur = ObservableReading("ddg", "currency", "EUR", currency.latest("EUR", "PLN"), "PLN", 4.0, 5.0, displayConfig("blue"))
      gbp = ObservableReading("ddg", "currency", "GBP", currency.latest("GBP", "PLN"), "PLN", 4.0, 6.0, displayConfig("purple"))
      xag = ObservableReading("ddg", "currency", "XAG", currency.latest("XAG", "PLN"), "PLN", 50.0, 200.0, displayConfig("gold"))
      xau = ObservableReading("ddg", "currency", "XAU", currency.latest("XAU", "PLN"), "PLN", 6000.0, 10000.0, displayConfig("silver"))
      btc = ObservableReading("ddg", "currency", "BTC", currency.latest("XBT", "PLN").map(_ / 1000.0), "kPLN", 100.0, 200.0, displayConfig("gray"))

      routes = Router(
        "/api/mesh" -> MeshApi.routes(mdns),
        "/api/data" -> DataApi.routes(config.node.name, config.node.model, config.node.group, List(usd, eur, gbp, xag, xau, btc)),
      ).orNotFound
      cors = CORS.policy.withAllowOriginAll(routes)
      app = if config.rest.logRequests then Logger.httpApp(true, true)(cors) else cors

      _ <- log.info("Starting mDNS responder...").background
      _ <- mdns.responder(config.node.name, config.mdns.port, config.mdns.dnsTTL).start
      _ <- mdns.scanner(config.mdns.scanInterval).start

      _ <- log.info("Starting observers...").background
      _ <- usd.observer(1.hour, 3).start
      _ <- eur.observer(1.hour, 3).start
      _ <- gbp.observer(1.hour, 3).start
      _ <- xag.observer(1.hour, 3).start
      _ <- xau.observer(1.hour, 3).start
      _ <- btc.observer(1.hour, 3).start

      _ <- log.info("Starting HTTP server...").background
      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(app)
          .resource
    yield ()
