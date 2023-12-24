package ssm

import cats.effect.*
import org.http4s.implicits.*
import org.http4s.server.{ Server, Router }
import org.http4s.server.middleware.Logger
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder
import org.http4s.server.middleware.*
import org.http4s.server.staticcontent.*

import ssm.api.*
import ssm.domain.*
import ssm.integrations.*
import ssm.Log.*
import ssm.service.*

object Main extends ResourceApp.Forever:
  private val log = getLogger

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      _ <- log.info(s"Started with: ${args.mkString(" ")}").resource
      _ <- log.info("Loading configuration...").resource
      config <- Config.load()
      nodeConfig <- Config.loadNodeConfigWithFallback(args.headOption)
      _ = Log.setLevel(nodeConfig.log.level)

      _ <- log.info("Booting services...").resource
      client <- BlazeClientBuilder[IO].resource
      mdns = MDNS(config.mdns.serviceName, config.mdns.serviceType)
      node = Node(nodeConfig, DDGCurrencyApi(client), NUTCli())

      routes = Router(
        "/api/mesh" -> MeshApi.routes(mdns),
        "/api/data" -> DataApi.routes(node),
        "/" -> fileService(FileService.Config(config.rest.dataPath))
      ).orNotFound
      cors = CORS.policy.withAllowOriginAll(routes)
      app = if config.rest.logRequests then Logger.httpApp(true, true)(cors) else cors

      _ <- log.info("Starting mDNS responder...").resource
      _ <- mdns.responder(nodeConfig.name, config.mdns.port, config.mdns.dnsTTL, config.mdns.retryInterval)
      _ <- mdns.scanner(config.mdns.scanInterval)

      _ <- log.info("Starting reading observers...").resource
      _ <- node.observer.start

      _ <- log.info("Starting HTTP server...").resource
      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(app)
          .resource
    yield ()
