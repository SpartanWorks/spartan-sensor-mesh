package ssm

import cats.effect.*
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
import sw.generated.model.WidgetConfig

object Main extends ResourceApp.Forever:

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      config <- Config.load()

      client <- BlazeClientBuilder[IO].resource
      mdns = MDNS(config.mdns.serviceName, "tcp")
      currency = DDGCurrencyApi(client)

      usd = ObservableReading("ddg", "currency", "USD", currency.latest("USD", "PLN"), "PLN", 4.0, 5.0, WidgetConfig("gauge"))
      eur = ObservableReading("ddg", "currency", "EUR", currency.latest("EUR", "PLN"), "PLN", 4.0, 5.0, WidgetConfig("gauge"))
      gbp = ObservableReading("ddg", "currency", "GBP", currency.latest("GBP", "PLN"), "PLN", 4.0, 6.0, WidgetConfig("gauge"))
      xag = ObservableReading("ddg", "currency", "XAG", currency.latest("XAG", "PLN"), "PLN", 50.0, 200.0, WidgetConfig("gauge"))
      xau = ObservableReading("ddg", "currency", "XAU", currency.latest("XAU", "PLN"), "PLN", 6000.0, 10000.0, WidgetConfig("gauge"))

      routes = Router(
        "/api/mesh" -> MeshApi.routes(mdns),
        "/api/data" -> DataApi.routes("Scala Agent", "jvm", "jvm", List(usd, eur, gbp, xag, xau)),
      ).orNotFound
      cors = CORS.policy.withAllowOriginAll(routes)
      app = if config.rest.logRequests then Logger.httpApp(true, true)(cors) else cors

      _ <- mdns.responder(config.mdns.nodeName, config.mdns.port, config.mdns.dnsTTL).start
      _ <- mdns.scanner(config.mdns.scanInterval).start
      _ <- usd.observer(10.minutes, 50).start
      _ <- eur.observer(10.minutes, 50).start
      _ <- gbp.observer(10.minutes, 50).start
      _ <- xag.observer(10.minutes, 50).start
      _ <- xau.observer(10.minutes, 50).start

      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(app)
          .resource
    yield ()
