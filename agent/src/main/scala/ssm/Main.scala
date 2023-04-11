package ssm

import cats.effect.*
import org.http4s.implicits.*
import org.http4s.server.Server
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder

import ssm.service.*

object Main extends ResourceApp.Forever:

  override def run(args: List[String]): Resource[IO, Unit] =
    for
      client <- BlazeClientBuilder[IO].resource

      service = DDGCurrencyApi(client)
      routes = Server.routes(service).orNotFound

      config <- Config.load()
      _ <- BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(routes)
          .resource
    yield ()
