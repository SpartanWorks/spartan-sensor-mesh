package ssm.service

import cats.effect.*
import cats.implicits.*
import io.circe.Json
import ssm.model.generated.*
import ssm.domain.*

import scala.concurrent.duration.*

trait Node:
  def observer: Resource[IO, Unit]
  def readings: IO[List[Reading]]
  def data: IO[Data]

object Node:
  def apply(config: Config, currencyApi: DDGCurrencyApi): Node =
    new NodeImpl(config, currencyApi)

  private[service] case class ObservableReadingConfig(reading: ObservableReading, samplingInterval: FiniteDuration, windowSize: Int)

  private[service] class NodeImpl(config: Config, currencyApi: DDGCurrencyApi) extends Node:

    private val readingsConf: List[ObservableReadingConfig] = config.sensors.map {
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
      readingsConf.map { conf =>
        conf.reading.observer(conf.samplingInterval, conf.windowSize)
      }.parSequence.map(_ => ())

    override def readings: IO[List[Reading]] =
      readingsConf.map { conf =>
        conf.reading.reading
      }.sequence

    override def data: IO[Data] =
      readings.map { rs =>
        Data(config.model, config.group, config.name, rs)
      }
