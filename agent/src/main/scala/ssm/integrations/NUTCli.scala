package ssm.integrations

import scala.util.Try

import cats.effect.*
import cats.implicits.*
import fs2.io.process.ProcessBuilder
import fs2.text

import ssm.Log.*


trait NUTCli:
  def fetchAll(ups: String): IO[List[NUTCli.NUTVariable]]
  def fetch(ups: String, variable: String): IO[Double]

object NUTCli:
  case class NUTVariable(name: String, value: Double)

  case class BadVariable(message: String) extends Throwable(message)
  case class CliError(message: String) extends Throwable(message)

  def apply(): NUTCli =
    new NUTCliImpl()

  private class NUTCliImpl() extends NUTCli:
    private val log = getLogger

    private def parseValue(value: String): IO[Double] =
      IO.fromTry(Try(value.toDouble))

    private def parseLine(line: String): IO[NUTVariable] =
       line.split(":").map(_.trim) match {
         case Array(name, value) => parseValue(value).map(NUTVariable(name, _))
         case _ => IO.raiseError(BadVariable(s"Could not parse variable value: ${line}"))
       }

    private def run(cmd: String, args: String*): IO[String] =
      ProcessBuilder(cmd, args.toList).spawn[IO].use { process =>
        process.stdout.through(text.utf8.decode).compile.string
      }.flatTap { output =>
        log.debug(s"Command '$cmd $args' resulted in: '$output'")
      }

    def fetchAll(ups: String): IO[List[NUTVariable]] =
      for
        output <- run("upsc", ups)
        variables <- output.split("\n").toList.map(parseLine).sequence
      yield variables

    def fetch(ups: String, variable: String): IO[Double] =
      for
        output <- run("upsc", ups, variable)
        value <- parseValue(output)
      yield value
