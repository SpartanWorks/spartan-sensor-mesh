package ssm

import com.typesafe.config.ConfigFactory
import io.circe.generic.auto.*
import io.circe.config.syntax.*

case class Rest(host: String, port: Int)

case class Config(rest: Rest)

object Config:

  private val config = ConfigFactory.load()

  val all = config.as[Config]("ssm")

