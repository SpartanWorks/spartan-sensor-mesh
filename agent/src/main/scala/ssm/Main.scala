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
import ssm.service.*

import scala.concurrent.duration.*

object Main extends ResourceApp.Forever:

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      config <- Config.load()

      client <- BlazeClientBuilder[IO].resource
      mdns = MDNS(config.mdns.serviceName, "tcp")
      currency = DDGCurrencyApi(client)

      displayConfig = Map("type" -> Json.fromString("gauge"))

      usd = ObservableReading("ddg", "currency", "USD", currency.latest("USD", "PLN"), "PLN", 4.0, 5.0, displayConfig)
      eur = ObservableReading("ddg", "currency", "EUR", currency.latest("EUR", "PLN"), "PLN", 4.0, 5.0, displayConfig)
      gbp = ObservableReading("ddg", "currency", "GBP", currency.latest("GBP", "PLN"), "PLN", 4.0, 6.0, displayConfig)
      xag = ObservableReading("ddg", "currency", "XAG", currency.latest("XAG", "PLN"), "PLN", 50.0, 200.0, displayConfig)
      xau = ObservableReading("ddg", "currency", "XAU", currency.latest("XAU", "PLN"), "PLN", 6000.0, 10000.0, displayConfig)
      btc = ObservableReading("ddg", "currency", "BTC", currency.latest("XBT", "PLN"), "PLN", 100000.0, 200000.0, displayConfig)

      routes = Router(
        "/api/mesh" -> MeshApi.routes(mdns),
        "/api/data" -> DataApi.routes("Scala Agent", "jvm", "jvm", List(usd, eur, gbp, xag, xau, btc)),
      ).orNotFound
      cors = CORS.policy.withAllowOriginAll(routes)
      app = if config.rest.logRequests then Logger.httpApp(true, true)(cors) else cors

      _ <- mdns.responder(config.mdns.nodeName, config.mdns.port, config.mdns.dnsTTL).start
      _ <- mdns.scanner(config.mdns.scanInterval).start
      _ <- usd.observer(1.hour, 3).start
      _ <- eur.observer(1.hour, 3).start
      _ <- gbp.observer(1.hour, 3).start
      _ <- xag.observer(1.hour, 3).start
      _ <- xau.observer(1.hour, 3).start
      _ <- btc.observer(1.hour, 3).start

      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(app)
          .resource
    yield ()
