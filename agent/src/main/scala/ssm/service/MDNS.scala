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
  def responder(name: String, port: Int, dnsTTL: FiniteDuration): Stream[IO, Unit]
  def scanner(scanInterval: FiniteDuration): Stream[IO, Set[MDNS.Node]]

  def nodes: IO[Set[MDNS.Node]]
}

object MDNS {
  case class Node(hostname: String, port: Int, address: InetAddress)

  case class BadAddress(message: String) extends Throwable(message)

  val CouldNotDetermineAddress = BadAddress("Could not determine the address of a node.")

  def apply(serviceName: String, serviceType: String): MDNS =
    new MDNSImpl(serviceName, serviceType)

  private[service] class MDNSImpl(serviceName: String, serviceType: String) extends MDNS {
    private val service = Zeroconf.Service(serviceName, serviceType)
    private val discoveredNodes = new AtomicReference[Set[Node]](Set.empty)
    private val log = Logger(getClass.getName)

    // NOTE Extracted for mocking in tests.
    protected def scanStream: Stream[IO, Zeroconf.Instance] =
      Zeroconf.scan[IO](service)

    def responder(name: String, port: Int, ttl: FiniteDuration): Stream[IO, Unit] =
      val instance = Zeroconf.Instance(service, name, port, name, Map.empty, Seq.empty)
      log.info(s"Registering service $serviceName ($serviceType:$port) under the name '$name'.")
      Zeroconf.register[IO](instance, ttl = ttl)

    def scanner(interval: FiniteDuration): Stream[IO, Set[Node]] =
      val single = scanStream
        .interruptAfter(interval)
        .compile
        .toList
        .flatMap { instances =>
          instances.map { instance =>
            for
              address <- IO.fromOption(instance.addresses.headOption)(CouldNotDetermineAddress)
            yield Node(instance.target, instance.port, address)
          }.sequence
        }
        .map { nodes =>
          val s = nodes.toSet
          log.debug(s"Updating mesh nodes: ${s}")
          discoveredNodes.set(s)
          s
        }

      log.info(s"Scanning for matching nodes every $interval.")
      Stream.repeatEval(single).metered(interval)

    def nodes: IO[Set[Node]] =
      IO(discoveredNodes.get())
  }
}
