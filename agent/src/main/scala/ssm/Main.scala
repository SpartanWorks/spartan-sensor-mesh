package ssm

import cats.effect.*
import org.http4s.implicits.*
import org.http4s.server.{ Server, Router }
import org.http4s.server.middleware.Logger
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder
import org.http4s.server.middleware.*

import ssm.api.*
import ssm.service.*

import scala.concurrent.duration.*

object Main extends ResourceApp.Forever:

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      config <- Config.load()

      client <- BlazeClientBuilder[IO].resource
      mdns = MDNS(config.mdns.serviceName, "tcp")
      currency = DDGCurrencyApi(client)

      routes = Router(
        "/api/mesh" -> MeshApi.routes(mdns),
        "/" -> Server.routes(currency)
      ).orNotFound
      cors = CORS.policy.withAllowOriginAll(routes)
      app = if config.rest.logRequests then Logger.httpApp(true, true)(cors) else cors

      _ <- mdns.responder(config.mdns.nodeName, config.mdns.port, config.mdns.dnsTTL).compile.resource.drain.start
      _ <- mdns.scanner(config.mdns.scanInterval).compile.resource.drain.start

      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(app)
          .resource
    yield ()
