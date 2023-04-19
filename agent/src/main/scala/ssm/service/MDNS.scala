package ssm.service

import cats.effect.*
import cats.implicits.*
import com.typesafe.scalalogging.Logger
import fs2.Stream
import fr.davit.scout.Zeroconf

import scala.concurrent.duration.FiniteDuration

import java.net.InetAddress


trait MDNS {
  def responder(name: String, port: Int, dnsTTL: FiniteDuration): Resource[IO, Unit]
  def scanner(scanInterval: FiniteDuration): Resource[IO, Unit]

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
    private val discoveredNodes = Ref.unsafe[IO, Set[Node]](Set.empty)
    private val log = Logger(getClass.getName)

    // NOTE These are extracted for mocking in tests.
    protected[service] def scanStream: Stream[IO, Zeroconf.Instance] =
      Zeroconf.scan[IO](service)

    protected[service] def responderStream(name: String, port: Int, ttl: FiniteDuration): Stream[IO, Unit] =
      val instance = Zeroconf.Instance(service, name, port, name, Map.empty, Seq.empty)
      log.info(s"Registering service $serviceName ($serviceType:$port) under the name '$name'.")
      Zeroconf.register[IO](instance, ttl = ttl)

    protected[service] def scannerStream(interval: FiniteDuration): Stream[IO, Set[Node]] =
      val single = for
        instances <- scanStream
          .interruptAfter(interval)
          .compile
          .toList
        nodes <- instances.map { instance =>
            for
              address <- IO.fromOption(instance.addresses.headOption)(CouldNotDetermineAddress)
            yield Node(instance.target, instance.port, address)
          }.sequence
        uniq = nodes.toSet
        _ = log.debug(s"Updating mesh nodes: ${uniq}")
        _ <- discoveredNodes.set(uniq)
      yield uniq

      log.info(s"Scanning for matching nodes every $interval.")
      Stream.repeatEval(single).meteredStartImmediately(interval)

    def responder(name: String, port: Int, ttl: FiniteDuration): Resource[IO, Unit] =
      responderStream(name, port, ttl).compile.resource.drain

    def scanner(interval: FiniteDuration): Resource[IO, Unit] =
      scannerStream(interval).compile.resource.drain

    def nodes: IO[Set[Node]] =
      discoveredNodes.get
  }
}
