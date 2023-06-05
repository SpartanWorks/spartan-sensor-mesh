package ssm

import cats.effect.*
import ch.qos.logback.classic.{Level => LogbackLevel}
import ch.qos.logback.classic.Logger
import org.typelevel.log4cats.*
import org.typelevel.log4cats.slf4j.*
import fs2.Stream
import ssm.model.generated.ConfigLog.*

object Log:
  def getLogger(using loggerName: LoggerName) = LoggerFactory[IO].getLogger

  def setLevel(level: Level) =
    org.slf4j.LoggerFactory.getLogger(org.slf4j.Logger.ROOT_LOGGER_NAME).asInstanceOf[Logger].setLevel(level match {
      case Debug => LogbackLevel.DEBUG
      case Info => LogbackLevel.INFO
      case Warning => LogbackLevel.WARN
      case Error => LogbackLevel.ERROR
      case Critical => LogbackLevel.OFF
    })

  extension [T](log: IO[T])
    def stream: Stream[IO, T] =
      Stream.eval(log)

  extension [T](log: IO[T])
    def resource: Resource[IO, T] =
      Resource.eval(log)

