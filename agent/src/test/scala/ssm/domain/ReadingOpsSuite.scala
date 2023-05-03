package ssm.domain

import cats.data.*
import cats.implicits.*
import fs2.Stream

import ssm.domain.ReadingOps

class ReadingOpsSuite extends munit.CatsEffectSuite:

  test("withStats should compute statistics of the stream an emit on each value") {
    val stream = Stream.emits(List(1.0, 2.0, 3.0))
    val expected = List(
      ReadingOps.Stats(1.0, 1.0, 0.0, 1, 1.0, 1.0, 1),
      ReadingOps.Stats(2.0, 1.5, 0.5, 2, 1.0, 2.0, 2),
      ReadingOps.Stats(3.0, 2.0, 2.0, 3, 1.0, 3.0, 3),
    )

    import ReadingOps.withStats

    stream.withStats().compile.toList.map { values =>
      assertEquals(values, expected)
    }

  }

  test("withStats should respect the sample limit") {
    val stream = Stream.emits((0 to 50).map { _ => 23.5 })
    val samples = 10
    val expected = (1 to 51).map { i =>
      ReadingOps.Stats(23.5, 23.5, 0.0, scala.math.min(samples, i), 23.5, 23.5, i)
    }.toList

    import ReadingOps.withStats

    stream.withStats(samples).compile.toList.map { values =>
      assertEquals(values, expected)
    }

  }
