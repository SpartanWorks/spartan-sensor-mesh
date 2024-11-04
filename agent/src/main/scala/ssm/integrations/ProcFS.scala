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

    private def rate(producer: IO[Double]): IO[Double] = {
      // FIXME Not great, could use a proper scan or something along those lines.
      val lastValueRef = Ref.unsafe[IO, Double](0)
      val lastTimeRef = Ref.unsafe[IO, Double](0)
      for {
        newValue <- producer
        newTime <- getUptime("uptime") // FIXME Also not great, but I love it.
        lastValue <- lastValueRef.get
        lastTime <- lastTimeRef.get
        _ <- lastValueRef.set(newValue)
        _ <- lastTimeRef.set(newTime)
      } yield (newValue - lastValue)/(newTime - lastTime)
    }


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

    def getSysClass(file: String, variable: String): IO[Double] =
      slurp(s"$file/$variable")
       .flatMap { file =>
         IO.fromTry(Try(file.toDouble))
       }

    def getThermal(file: String, variable: String): IO[Double] =
      getSysClass(file, variable).map { r =>
         r / 1000
       }

    def getNetwork(file: String, variable: String): IO[Double] =
      variable match {
        case "rx" | "tx" =>
          rate(getSysClass(file, s"statistics/${variable}_bytes").map { v => v / 1024 })
        case _ =>
          IO.raiseError(BadVariable(s"$variable doesn't name a valid network value: rx or tx."))
      }

    def get(file: String, variable: String): IO[Double] =
      file match {
        case "/proc/uptime" => getUptime(variable)
        case "/proc/loadavg" => getLoadavg(variable)
        case f if "/sys/class/thermal/.+".r.matches(f) => getThermal(f, variable)
        case f if "/sys/class/net/.+".r.matches(f) => getNetwork(f, variable)
        case _ => IO.raiseError(BadFile(s"The file $file is not supported!"))
      }
