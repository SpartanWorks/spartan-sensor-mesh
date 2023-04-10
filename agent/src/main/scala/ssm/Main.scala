package ssm

import cats.effect.*
import org.http4s.implicits.*
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder

import ssm.service.*

object Main extends IOApp:

  override def run(args: List[String]): IO[ExitCode] =
    for
      config <- IO.fromEither(Config.all)
      server <- BlazeClientBuilder[IO].resource.use { client =>
        val service = DDGCurrencyApi(client)
        BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(Server.routes(service).orNotFound)
          .serve
          .compile
          .drain
          .as(ExitCode.Success)
      }
    yield server
