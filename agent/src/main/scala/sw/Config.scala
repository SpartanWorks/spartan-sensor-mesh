package sw

import com.typesafe.config.ConfigFactory
import io.circe.generic.auto.*
import io.circe.config.syntax.*

case class Rest(host: String, port: Int)

case class FreeCurrencyApi(apiKey: String)

case class Config(rest: Rest, freeCurrencyApi: FreeCurrencyApi)

object Config:

  private val config = ConfigFactory.load()

  val all = config.as[Config]("sw")

