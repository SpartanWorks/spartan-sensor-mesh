package ssm.service

import cats.effect.*
import cats.implicits.*
import com.typesafe.scalalogging.Logger
import fs2.Stream
import fr.davit.scout.Zeroconf

import scala.concurrent.duration.FiniteDuration

import java.net.InetAddress
import java.util.concurrent.atomic.AtomicReference


trait MDNS {
  def responder(name: String, port: Int, dnsTTL: FiniteDuration): Resource[IO, Unit]
  def scanner(scanInterval: FiniteDuration): Resource[IO, Unit]

  def nodes: IO[Set[MDNS.Node]]
}

object MDNS {
  case class Node(hostname: String, port: Int, address: InetAddress)

  case class BadAddress(message: String) extends Throwable(message)

  def apply(serviceName: String, serviceType: String): MDNS =
    new MDNSImpl(serviceName, serviceType)

  private class MDNSImpl(serviceName: String, serviceType: String) extends MDNS {
    private val service = Zeroconf.Service(serviceName, serviceType)
    private val discoveredNodes = new AtomicReference[Set[Node]](Set.empty)
    private val log = Logger(getClass.getName)

    def responder(name: String, port: Int, ttl: FiniteDuration): Resource[IO, Unit] =
      val instance = Zeroconf.Instance(service, name, port, name, Map.empty, Seq.empty)
      log.info(s"Registering service $serviceName ($serviceType:$port) under the name '$name'.")
      Zeroconf.register[IO](instance, ttl = ttl).compile.resource.drain

    def scanner(interval: FiniteDuration): Resource[IO, Unit] =
      val single = Zeroconf
        .scan[IO](service)
        .interruptAfter(interval)
        .compile
        .toList
        .flatMap { instances =>
          instances.map { instance =>
            for
              address <- IO.fromOption(instance.addresses.headOption)(BadAddress("Could not determine the address of a node."))
            yield Node(instance.target, instance.port, address)
          }.sequence
        }
        .map { nodes =>
          log.debug(s"Updating mesh nodes: ${nodes.toSet}")
          discoveredNodes.set(nodes.toSet)
        }

      log.info(s"Scanning for matching nodes every $interval.")
      Stream.repeatEval(single).metered(interval).compile.resource.drain

    def nodes: IO[Set[Node]] =
      IO(discoveredNodes.get())
  }
}
