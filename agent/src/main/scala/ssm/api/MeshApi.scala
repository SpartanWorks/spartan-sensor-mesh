package ssm.api

import cats.effect.*
import org.http4s.*
import org.http4s.circe.*
import org.http4s.circe.CirceEntityEncoder.circeEntityEncoder
import org.http4s.dsl.io.*

import ssm.model.generated.Mesh
import ssm.model.generated.Mesh.given
import ssm.service.MDNS

object MeshApi:

  def routes(mdns: MDNS) = HttpRoutes.of[IO] {

    case GET -> Root =>
      for
        nodes <- mdns.nodes
        mesh = nodes.map { n => Mesh(n.hostname, n.address.getHostAddress(), n.port) }
        response <- Ok(mesh)
      yield response

  }

