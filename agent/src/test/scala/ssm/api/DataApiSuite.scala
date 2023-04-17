package ssm.api

import cats.effect.*
import cats.syntax.all.*
import fs2.Stream
import org.http4s.*
import org.http4s.circe.*
import org.http4s.circe.CirceEntityDecoder.circeEntityDecoder
import org.http4s.dsl.io.*
import org.http4s.implicits.*
import ssm.service.Sampler
import sw.generated.model.*
import sw.generated.model.Data.given

import scala.concurrent.duration.FiniteDuration

class DataApiSuite extends munit.CatsEffectSuite:
  val request = Request[IO](Method.GET, uri"/")

  def mockSampler(mockStatus: Sampler.Status, mockErrors: Sampler.Errors, mockStats: Sampler.Stats) =
    new Sampler:
      def sampler(samplingInterval: FiniteDuration, windowSize: Int): Resource[IO, Unit] =
        Resource.eval(IO.unit)

      def status: IO[Sampler.Status] =
        IO.pure(mockStatus)

      def stats: IO[Sampler.Stats] =
        IO.pure(mockStats)

      def errors: IO[Sampler.Errors] =
        IO.pure(mockErrors)

  test("Handles empty data") {
    DataApi.routes("name", "model", "group", List.empty).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[Data].assertEquals(Data("model", "group", "name", List.empty))
    }
  }

  test("Handles sampler data correctly") {
    val expectedStatus = Sampler.Status.Ok
    val expectedErrors = Sampler.Errors(23, "Oops")
    val expectedStats = Sampler.Stats(1, 2, 3, 4, 5, 6, 7)
    val sampler = mockSampler(
      expectedStatus,
      expectedErrors,
      expectedStats
    )
    DataApi.routes("name", "model", "group", List(sampler)).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[Data].map { data =>
        assertEquals(data.readings(0).errors.intValue(), expectedErrors.count)
        assertEquals(data.readings(0).lastError, expectedErrors.lastMessage)
        assertEquals(data.readings(0).measurements.intValue(), expectedStats.total)
        assertEquals(data.readings(0).value.last.doubleValue(), expectedStats.latest)
        assertEquals(data.readings(0).value.stats, ReadingValueStats(
          expectedStats.mean,
          expectedStats.variance,
          expectedStats.count,
          Some(expectedStats.min),
          Some(expectedStats.max)
        ))
      }
    }
  }

  test("Handles multiple samplers correctly") {
    val sampler1 = mockSampler(
      Sampler.Status.Ok,
      Sampler.Errors(23, "Oops"),
      Sampler.Stats(1, 2, 3, 4, 5, 6, 7)
    )
    val sampler2 = mockSampler(
      Sampler.Status.Ok,
      Sampler.Errors(5, "Oof"),
      Sampler.Stats(1, 2, 3, 4, 5, 6, 7)
    )
    DataApi.routes("name", "model", "group", List(sampler1, sampler2)).orNotFound.run(request).flatMap { response =>
      assertEquals(response.status, Status.Ok)
      response.as[Data].map { data =>
        assertEquals(data.readings.length, 2)
        assertEquals(data.readings(0).errors.intValue(), 23)
        assertEquals(data.readings(0).lastError, "Oops")
        assertEquals(data.readings(1).errors.intValue(), 5)
        assertEquals(data.readings(1).lastError, "Oof")
      }
    }
  }

  test("Doesn't handle other requsets") {
    val request = Request[IO](Method.GET, uri"/mesh")

    DataApi.routes("name", "model", "group", List.empty).orNotFound.run(request).map { response =>
      assertEquals(response.status, Status.NotFound)
    }
  }
