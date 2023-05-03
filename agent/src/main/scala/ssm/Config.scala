package ssm

import cats.effect.*
import cats.implicits.*
import cats.data.OptionT
import com.typesafe.config.ConfigFactory
import fs2.Stream
import fs2.io.file.{Files, Path}
import fs2.text
import io.circe.generic.auto.*
import io.circe.config.syntax.*
import io.circe.parser.decode
import ssm.model.generated.{ Config => Node}

import scala.concurrent.duration.FiniteDuration

case class Rest(host: String, port: Int, logRequests: Boolean)

case class MDNS(serviceName: String, serviceType: String, port: Int, scanInterval: FiniteDuration, dnsTTL: FiniteDuration)

case class Config(rest: Rest, mdns: MDNS)

object Config:

  case class IncompleteConfig(message: String) extends Throwable(message)

  def load(): Resource[IO, Config] =
    val config = IO.fromEither(ConfigFactory.load().as[Config]("ssm"))
    Resource.eval(config)

  def loadNodeConfig(): Resource[IO, Node] =
    val node = IO.fromEither(ConfigFactory.load().as[Node]("ssm.node")).handleErrorWith { error =>
      IO.raiseError(IncompleteConfig("Could not find path `ssm.node` in `application.conf`."))
    }
    Resource.eval(node)

  def loadFromFile(filename: String): Resource[IO, Node] =
    Files[IO]
      .readAll(Path(filename))
      .through(text.utf8.decode)
      .flatMap { conf =>
        Stream.eval(IO.fromEither(decode[Node](conf)))
      }
      .compile
      .resource
     .lastOrError

  def loadNodeConfigWithFallback(filename: Option[String]): Resource[IO, Node] =
    val n = filename.map(Config.loadFromFile(_)).sequence
    OptionT(n).getOrElseF(Config.loadNodeConfig())
