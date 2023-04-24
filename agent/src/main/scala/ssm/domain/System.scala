package ssm.domain

import cats.effect.*
import cats.implicits.*
import io.circe.Json
import ssm.model.generated.*
import ssm.service.*

import scala.concurrent.duration.*

trait System:
  def observer: Resource[IO, Unit]
  def readings: IO[List[Reading]]

object System:
  def assemble(config: Config, mdns: MDNS, currencyApi: DDGCurrencyApi): System =
    new SystemImpl(config, mdns, currencyApi)

  private case class ObservableReadingConfig(reading: ObservableReading, samplingInterval: FiniteDuration, windowSize: Int)

  private class SystemImpl(config: Config, mdns: MDNS, currencyApi: DDGCurrencyApi) extends System:

    private val system: List[ObservableReadingConfig] = config.sensors.map {
      case ConfigSensorsInner("currency", true, samplingInterval, _, readings) =>
        readings.map { case ReadingConfig(target, source, averaging, widgetConfig) =>
          ObservableReadingConfig(
            ObservableReading("ddg", "currency", source, currencyApi.latest(source, target), target, 0, 1000, widgetConfig),
            samplingInterval.toInt.millis,
            averaging.toInt
          )
        }

      case _ => Nil
    }.flatten

    override def observer: Resource[IO, Unit] =
      system.map { conf =>
        conf.reading.observer(conf.samplingInterval, conf.windowSize)
      }.parSequence.map(_ => ())

    override def readings: IO[List[Reading]] =
      system.map { conf =>
        conf.reading.reading
      }.sequence
