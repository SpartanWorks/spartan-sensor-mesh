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
  def run: Resource[IO, Unit]
  def nodes: IO[Set[MDNS.Node]]
}

object MDNS {
  case class Node(hostname: String, port: Int, address: InetAddress)

  case class BadAddress(message: String) extends Throwable(message)

  def apply(name: String, serviceName: String, serviceType: String, port: Int, scanInterval: FiniteDuration, dnsTTL: FiniteDuration): MDNS =
    new MDNSImpl(name, serviceName, serviceType, port, scanInterval, dnsTTL)

  private class MDNSImpl(name: String, serviceName: String, serviceType: String, port: Int, interval: FiniteDuration, ttl: FiniteDuration) extends MDNS {
    private val service = Zeroconf.Service(serviceName, serviceType)
    private val instance = Zeroconf.Instance(service, name, port, name, Map.empty, Seq.empty)
    private val ref = new AtomicReference[Set[Node]](Set.empty)
    private val log = Logger(getClass.getName)

    def run: Resource[IO, Unit] = {
      val register = Zeroconf.register[IO](instance, ttl = ttl)

      val scan = Zeroconf
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
          ref.set(nodes.toSet)
        }

      log.info(s"Registering service $serviceName ($serviceType:$port) under the name '$name'.")
      log.info(s"Scanning for matching nodes every $interval.")
      Stream.repeatEval(scan).metered(interval).concurrently(register).compile.resource.drain
     }

    def nodes: IO[Set[Node]] =
      IO(ref.get())
  }
}
