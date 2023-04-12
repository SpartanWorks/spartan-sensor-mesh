package ssm

import cats.effect.*
import org.http4s.implicits.*
import org.http4s.server.Server
import org.http4s.server.middleware.Logger
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder
import ssm.service.*

import scala.concurrent.duration.*

object Main extends ResourceApp.Forever:

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      config <- Config.load()

      client <- BlazeClientBuilder[IO].resource
      mdns = MDNS(config.mdns.serviceName, "tcp")
      currency = DDGCurrencyApi(client)
      routes = Server.routes(currency).orNotFound

      _ <- mdns.responder(config.mdns.nodeName, config.mdns.port, config.mdns.dnsTTL).start
      _ <- mdns.scanner(config.mdns.scanInterval).start

      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(if config.rest.logRequests then Logger.httpApp(true, true)(routes) else routes)
          .resource
    yield ()
