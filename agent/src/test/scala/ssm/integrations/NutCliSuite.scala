package ssm.integrations

import cats.effect.*
import io.circe.*
import io.circe.syntax.*
import io.circe.generic.auto.*
import fs2.Stream
import org.http4s.*
import org.http4s.client.*
import org.http4s.client.dsl.io.*
import org.http4s.circe.CirceEntityEncoder.*
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import org.http4s.server.*
import org.http4s.server.blaze.*
import org.http4s.headers.*
import org.http4s.syntax.all.*
import munit.*

class NUTCliSuite extends CatsEffectSuite:

  val ups = "usbhid@localhost"

  val mockedShell = new NUTCli.Shell() {
    override def exec(cmd: String, args: String*): IO[String] =
      args.toList match {
        case usp :: Nil =>
          IO("""
          |battery.charge: 100
          |battery.charge.low: 10
          |battery.charge.warning: 20
          |""".stripMargin)
        case ups :: "battery.charge" :: Nil =>
          IO("100\n")
        case _ =>
          IO("Error: old-style variable names are not supported\n")
        }
  }

  val badShell = new NUTCli.Shell() {
    override def exec(cmd: String, args: String*): IO[String] =
      IO("zsh: command not found: upsc")
  }

  val cli = new NUTCli.NUTCliImpl(mockedShell)
  val badCli = new NUTCli.NUTCliImpl(badShell)

  test("Fetches all the variables.") {
    cli.fetchAll(ups).map { result =>
      assertEquals(result, List(
        NUTCli.NUTVariable("battery.charge", 100),
        NUTCli.NUTVariable("battery.charge.low", 10),
        NUTCli.NUTVariable("battery.charge.warning", 20)
      ))
    }
  }

  test("Handles bad output when fetching all variables.") {
    badCli.fetchAll(ups).map { result =>
    assert(false)
    }.handleError { error =>
      assertEquals(error, NUTCli.BadVariable("Could not parse variable: zsh: command not found: upsc"))
    }
  }

  test("Fetches the right variable.") {
    cli.fetch(ups, "battery.charge").map { result =>
      assertEquals(result, 100.0)
    }
  }

  test("Handles invalid variable names.") {
    cli.fetch(ups, "battery.charge.dunno").map { result =>
      assert(false)
    }.handleError { error =>
      assertEquals(error, NUTCli.BadVariable("Could not parse variable value: Error: old-style variable names are not supported"))
    }
  }

  test("Handles bad upsc output when fetching a single variable.") {
    badCli.fetch(ups, "battery.charge.dunno").map { result =>
      assert(false)
    }.handleError { error =>
      assertEquals(error, NUTCli.BadVariable("Could not parse variable value: zsh: command not found: upsc"))
    }
  }
