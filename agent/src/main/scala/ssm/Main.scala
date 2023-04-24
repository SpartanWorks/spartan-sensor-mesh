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
import ssm.Log.*
import ssm.service.*

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
      system = System.assemble(???, mdns, currency)

      routes = Router(
        "/api/mesh" -> MeshApi.routes(mdns),
        "/api/data" -> DataApi.routes(config.node.name, config.node.model, config.node.group, system),
      ).orNotFound
      cors = CORS.policy.withAllowOriginAll(routes)
      app = if config.rest.logRequests then Logger.httpApp(true, true)(cors) else cors

      _ <- log.info("Starting mDNS responder...").background
      _ <- mdns.responder(config.node.name, config.mdns.port, config.mdns.dnsTTL).start
      _ <- mdns.scanner(config.mdns.scanInterval).start

      _ <- log.info("Starting reading observers...").background
      _ <- system.observer.start

      _ <- log.info("Starting HTTP server...").background
      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(app)
          .resource
    yield ()
