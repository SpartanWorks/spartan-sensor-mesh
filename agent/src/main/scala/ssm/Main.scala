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
      mdns = MDNS(config.mdns.nodeName, config.mdns.serviceName, "tcp", config.mdns.port, config.mdns.scanInterval)
      currency = DDGCurrencyApi(client)
      routes = Server.routes(currency).orNotFound

      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(if config.rest.logRequests then Logger.httpApp(true, true)(routes) else routes)
          .resource
      _ <- mdns.run
    yield ()
