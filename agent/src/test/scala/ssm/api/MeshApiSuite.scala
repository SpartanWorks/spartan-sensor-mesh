package ssm.api

import cats.effect.*
import cats.syntax.all.*
import fs2.Stream
import org.http4s.*
import org.http4s.circe.*
import org.http4s.circe.CirceEntityDecoder.circeEntityDecoder
import org.http4s.dsl.io.*
import org.http4s.implicits.*

import sw.generated.model.Mesh
import sw.generated.model.Mesh.given
import ssm.service.MDNS

import scala.concurrent.duration.FiniteDuration
import java.net.InetAddress

class MeshApiSuite extends munit.CatsEffectSuite:
  val request = Request[IO](Method.GET, uri"/")

  def mockService(expectedNodes: Set[MDNS.Node]) = new MDNS:
    def responder(name: String, port: Int, dnsTTL: FiniteDuration): Stream[IO, Unit] =
      Stream.unit[IO]

    def scanner(scanInterval: FiniteDuration): Stream[IO, Set[MDNS.Node]] =
      Stream.emits(Seq(Set.empty[MDNS.Node]))

    def nodes: IO[Set[MDNS.Node]] =
      IO.pure(expectedNodes)

  test("Returns a list of mesh nodes") {
    val service = mockService(Set(
      MDNS.Node("google", 23, InetAddress.getByName("8.8.8.8")),
      MDNS.Node("cloudflare", 80, InetAddress.getByName("1.1.1.1"))
    ))
    val expected = Set(
      Mesh("google", "8.8.8.8", 23),
      Mesh("cloudflare", "1.1.1.1", 80)
    )

    MeshApi.routes(service).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[Set[Mesh]].assertEquals(expected)
    }
  }

  test("Handles empty mesh") {
    val service = mockService(Set.empty[MDNS.Node])

    MeshApi.routes(service).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[Set[Mesh]].assertEquals(Set.empty[Mesh])
    }
  }

  test("Doesn't handle other requsets") {
    val service = mockService(Set.empty[MDNS.Node])
    val request = Request[IO](Method.GET, uri"/data")

    MeshApi.routes(service).orNotFound.run(request).map { response =>
      assertEquals(response.status, Status.NotFound)
    }
  }
