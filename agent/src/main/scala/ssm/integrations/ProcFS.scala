package ssm.integrations

import scala.util.{Try, Failure}

import cats.effect.*
import cats.implicits.*
import fs2.io.file.*
import fs2.text
import fs2.Stream

import ssm.Log.*


trait ProcFS:
  def get(file: String, variable: String): IO[Double]

object ProcFS:
  case class BadFile(message: String) extends Throwable(message)
  case class BadVariable(message: String) extends Throwable(message)

  def apply(): ProcFS =
    new ProcFSImpl()

  private[integrations] class ProcFSImpl() extends ProcFS:
    private val log = getLogger

    private def slurp(filename: String): IO[String] =
      Files[IO]
        .readAll(Path(filename))
        .through(text.utf8.decode)
        .compile
        .lastOrError

    def getUptime(variable: String): IO[Double] =
      variable match {
        case "uptime" | "idle" =>
          slurp("/proc/uptime")
            .flatMap { file =>
              val v = file.split(" ")(variable match {
                case "uptime" => 0
                case "idle" => 1
              })
              IO.fromTry(Try(v.toDouble))
            }
        case _ =>
          IO.raiseError(BadVariable(s"$variable doesn't name a valid uptime value: uptime, idle."))
      }

    def getLoadavg(variable: String): IO[Double] =
      variable match {
        case "1min" | "5min" | "15min" =>
          slurp("/proc/loadavg")
            .flatMap { file =>
              val v = file.split(" ")(variable match {
                case "1min" => 0
                case "5min" => 1
                case "15min" => 2
              })
              IO.fromTry(Try(v.toDouble))
            }
        case _ =>
          IO.raiseError(BadVariable(s"$variable doesn't name a valid load average value: 1min, 5min or 15min."))
    }

    def get(file: String, variable: String): IO[Double] =
      file match {
        case "/proc/uptime" => getUptime(variable)
        case "/proc/loadavg" => getLoadavg(variable)
        case _ => IO.raiseError(BadFile(s"The file $file is not supported!"))
      }
