package ssm.api

import cats.effect.*
import org.http4s.*
import org.http4s.circe.*
import org.http4s.circe.CirceEntityEncoder.circeEntityEncoder
import org.http4s.dsl.io.*

import sw.generated.model.Mesh
import sw.generated.model.Mesh.given
import ssm.service.MDNS

object MeshApi:

  def routes(mdns: MDNS) = HttpRoutes.of[IO] {

    case GET -> Root =>
      for
        nodes <- mdns.nodes
        mesh = nodes.map { n => Mesh(n.hostname, n.address.toString, n.port) }
        response <- Ok(mesh)
      yield response

  }

