package ssm.service

import cats.effect.*
import cats.implicits.*
import munit.*
import fs2.Stream
import fr.davit.scout.Zeroconf

import scala.concurrent.duration.*

import java.net.InetAddress

class MDNSSuite extends CatsEffectSuite:
  def mockMDNS(instances: List[Zeroconf.Instance]): MDNS =
    new MDNS.MDNSImpl("test", "service"):
        override def scanStream: Stream[IO, Zeroconf.Instance] =
          Stream.emits(instances)

  test("Should respond with an empty node list initially") {
    val mdns = mockMDNS(List.empty)

    mdns.nodes.map { nodes =>
      assertEquals(nodes, Set.empty[MDNS.Node])
    }
  }

  test("Should respond with a list of nodes collected during scanning") {
    val service = Zeroconf.Service("name", "service")
    val mdns = mockMDNS(List(
      Zeroconf.Instance(service, "cloudflare", 23, "cloudflare", Map.empty, Seq(InetAddress.getByName("1.1.1.1"))),
      Zeroconf.Instance(service, "google", 5, "google", Map.empty, Seq(InetAddress.getByName("8.8.8.8"))),
    ))
    val expected = Set(
      MDNS.Node("google", 5, InetAddress.getByName("8.8.8.8")),
      MDNS.Node("cloudflare", 23, InetAddress.getByName("1.1.1.1")),
    )

    // FIXME Flaky-city, population: you.
    mdns.scanner(100.micros).start.use { f =>
      for
        _ <- IO.sleep(1.second)
        nodes <- mdns.nodes
        _ = assertEquals(nodes, expected)
        _ <- f.cancel.use(_ => IO.unit)
      yield ()
    }
  }
