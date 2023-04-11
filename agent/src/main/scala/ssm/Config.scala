package ssm

import cats.effect.*
import com.typesafe.config.ConfigFactory
import io.circe.generic.auto.*
import io.circe.config.syntax.*

case class Rest(host: String, port: Int)

case class Config(rest: Rest)

object Config:

  def load() =
    val config = IO.fromEither(ConfigFactory.load().as[Config]("ssm"))
    Resource.eval(config)

