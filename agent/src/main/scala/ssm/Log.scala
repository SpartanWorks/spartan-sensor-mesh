package ssm

import cats.effect.*
import org.typelevel.log4cats.*
import org.typelevel.log4cats.slf4j.*
import fs2.Stream

object Log:
  def getLogger(using loggerName: LoggerName) = LoggerFactory[IO].getLogger

  extension [T](log: IO[T])
    def stream: Stream[IO, T] =
      Stream.eval(log)

