package ssm

import cats.effect.*
import com.typesafe.config.ConfigFactory
import io.circe.generic.auto.*
import io.circe.config.syntax.*

import scala.concurrent.duration.FiniteDuration

case class Rest(host: String, port: Int, logRequests: Boolean)

case class Node(name: String, model: String, group: String)

case class MDNS(serviceName: String, serviceType: String, port: Int, scanInterval: FiniteDuration, dnsTTL: FiniteDuration)

case class Config(node: Node, rest: Rest, mdns: MDNS)

object Config:

  def load() =
    val config = IO.fromEither(ConfigFactory.load().as[Config]("ssm"))
    Resource.eval(config)

