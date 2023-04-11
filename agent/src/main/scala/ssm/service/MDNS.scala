package ssm.service

import cats.effect.*
import cats.implicits.*
import com.typesafe.scalalogging.Logger
import fs2.Stream
import fr.davit.scout.*

import scala.concurrent.duration.*

import java.net.InetAddress
import java.util.concurrent.atomic.AtomicReference

case class Node(host: String, port: Int, address: InetAddress)

case class MDNSBadAddress(message: String) extends Throwable(message)

trait MDNS {
  def run: Resource[IO, Unit]
  def mesh: IO[Set[Node]]
}

object MDNS {
  def apply(name: String, serviceName: String, serviceType: String, port: Int, scanInterval: FiniteDuration): MDNS =
    new MDNSImpl(name, serviceName, serviceType, port, scanInterval)

  private class MDNSImpl(name: String, serviceName: String, serviceType: String, port: Int, interval: FiniteDuration) extends MDNS {
    private val service = Zeroconf.Service(serviceName, serviceType)
    private val instance = Zeroconf.Instance(service, name, port, name, Map.empty, Seq.empty)
    private val ref = new AtomicReference[Set[Node]](Set.empty)
    private val log = Logger(getClass.getName)

    def run: Resource[IO, Unit] = {
      val register = Zeroconf.register[IO](instance)

      val scan = Zeroconf
        .scan[IO](service)
        .interruptAfter(interval)
        .compile
        .toList
        .flatMap { instances =>
          instances.map { instance =>
            for
              address <- IO.fromOption(instance.addresses.headOption)(MDNSBadAddress("Could not determine the address of a node."))
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

    def mesh: IO[Set[Node]] =
      IO(ref.get())
  }
}
