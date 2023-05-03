package ssm.service

import cats.effect.*
import cats.implicits.*
import munit.*
import ssm.domain.ObservableReading
import ssm.integrations.DDGCurrencyApi
import ssm.model.generated.*

import scala.concurrent.duration.*

class NodeSuite extends CatsEffectSuite:

  def mockCurrencyApi(): DDGCurrencyApi =
    new DDGCurrencyApi {
      override def latest(base: String, targets: String): IO[Double] =
        IO.never
    }

  def mockReadings(rs: List[Node.ObservableReadingConfig]): Node.ReadingsBuilder =
    new Node.ReadingsBuilder(List.empty, mockCurrencyApi()) {
      override def build = rs
    }

  case class ObserverConfig(samplingInterval: FiniteDuration, windowSize: Int)

  def mockReading(value: BigDecimal, samplingInterval: FiniteDuration, windowSize: Int, ob: ObserverConfig => IO[Unit] = _ => IO.unit): Node.ObservableReadingConfig =
    Node.ObservableReadingConfig(
        new ObservableReading {
          override def observer(samplingInterval: FiniteDuration, windowSize: Int): Resource[IO, Unit] =
            ob(ObserverConfig(samplingInterval, windowSize)).background.map(_ => ())

          override def reading: IO[Reading] =
            IO(Reading("model", "type", "name", "status", 0, "lastError", 1, ReadingValue(
              value, "unit", ReadingValueStats(0, 0, 0, Some(0), Some(1)), ReadingValueRange(0, 1)
            ), Map.empty))
        },
        samplingInterval,
        windowSize
      )


  test("Should handle empty readings") {
    val config = Config("model", "group", "name", "password", ConfigLog(ConfigLog.Debug, Map.empty), List.empty)
    val service = new Node.NodeImpl(config, mockReadings(List.empty))

    assertIO(service.data, Data(config.model, config.group, config.name, List.empty))
  }

  test("Should fetch all readings values") {
    val config = Config("model", "group", "name", "password", ConfigLog(ConfigLog.Debug, Map.empty), List.empty)
    val service = new Node.NodeImpl(config, mockReadings(List(
      mockReading(23, 10.millis, 1),
      mockReading(5, 10.millis, 1),
    )))

    service.readings.map { rs =>
      assert(rs.exists(r => r.value.last == 23) && rs.exists(r => r.value.last == 5))
    }
  }

  test("Should run all readings observers") {
    val config = Config("model", "group", "name", "password", ConfigLog(ConfigLog.Debug, Map.empty), List.empty)

    var reading1Config: Option[ObserverConfig] = None
    var reading2Config: Option[ObserverConfig] = None

    val service = new Node.NodeImpl(config, mockReadings(List(
      mockReading(23, 10.millis, 1, { conf =>
        reading1Config = Some(conf)
        IO.unit
      }),
      mockReading(5, 1.millis, 10, { conf =>
        reading2Config = Some(conf)
        IO.unit
      }),
    )))

    service.observer.use { _ =>
     IO {
       assertEquals(reading1Config, Some(ObserverConfig(10.millis, 1)))
       assertEquals(reading2Config, Some(ObserverConfig(1.millis, 10)))
     }
    }
  }

  test("Should correctly build readings from config") {
    val reading1 = ReadingConfig("type1", "name1", 23, Map.empty)
    val reading2 = ReadingConfig("type2", "name2", 5, Map.empty)

    val configs: List[(List[ConfigSensorsInner], Int)] = List(
      // No config
      (List.empty, 0),
      // Unknown, disabled
      (List(ConfigSensorsInner("unknown", false, 100, Map.empty, List(reading1, reading2))), 0),
      // Unknown, enabled, no readings.
      (List(ConfigSensorsInner("unknown", true, 100, Map.empty, List.empty)), 0),
      // Unknown, enabled
      (List(ConfigSensorsInner("unknown", true, 100, Map.empty, List(reading1, reading2))), 0),
      // Known, disabled
      (List(ConfigSensorsInner("currency", false, 100, Map.empty, List(reading1, reading2))), 0),
      // Known, enabled, no readings
      (List(ConfigSensorsInner("currency", true, 100, Map.empty, List.empty)), 0),
      // Known, enabled, readings.
      (List(ConfigSensorsInner("currency", true, 100, Map.empty, List(reading1, reading2))), 2),
    )

    configs.foreach { (config, expected) =>
      val builder = Node.ReadingsBuilder(config, mockCurrencyApi())
      assertEquals(builder.build.length, expected)
    }
  }
