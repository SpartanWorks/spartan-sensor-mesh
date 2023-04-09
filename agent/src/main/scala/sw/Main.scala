package sw

import cats.effect.*
import org.http4s.implicits.*
import org.http4s.blaze.server.BlazeServerBuilder
import org.http4s.blaze.client.BlazeClientBuilder

import sw.service.*

object Main extends IOApp:

  override def run(args: List[String]): IO[ExitCode] =
    for
      config <- IO.fromEither(Config.all)
      server <- BlazeClientBuilder[IO].resource.use { client =>
        val service = FreeCurrencyApi(client, config.freeCurrencyApi.apiKey)
        BlazeServerBuilder[IO]
          .bindHttp(config.rest.port, config.rest.host)
          .withHttpApp(Server.routes(service).orNotFound)
          .serve
          .compile
          .drain
          .as(ExitCode.Success)
      }
    yield server
