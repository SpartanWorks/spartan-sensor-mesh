package ssm.service

import cats.effect.*
import cats.implicits.*
import io.circe.Json
import ssm.domain.ObservableReading
import ssm.integrations.{DDGCurrencyApi, NUTCli, ProcFS}
import ssm.model.generated.*

import scala.concurrent.duration.*

trait Node:
  def observer: Resource[IO, Unit]
  def readings: IO[List[Reading]]
  def data: IO[Data]

object Node:
  def apply(config: Config, currencyApi: DDGCurrencyApi, nutCli: NUTCli, procFs: ProcFS): Node =
    new NodeImpl(config, new ReadingsBuilder(config.sensors, currencyApi, nutCli, procFs))

  private[service] case class ObservableReadingConfig(reading: ObservableReading, samplingInterval: FiniteDuration, windowSize: Int)

  // FIXME This should ideally be moved into separate integartion builders that are passed here.
  private[service] class ReadingsBuilder(config: List[ConfigSensorsInner], currencyApi: DDGCurrencyApi, nutCli: NUTCli, procFs: ProcFS):
    def build: List[ObservableReadingConfig] =
      config.flatMap {
        case ConfigSensorsInner("currency", true, samplingInterval, _, readings) =>
          readings.map {
            case ReadingConfig(target, source, averaging, widgetConfig) =>
              ObservableReadingConfig(
                ObservableReading("ddg", "currency", source, currencyApi.latest(source, target), target, 0, 1000, widgetConfig),
                samplingInterval.toInt.millis,
                averaging.toInt
              )
          }
        case ConfigSensorsInner("network-ups-tools", true, samplingInterval, connection, readings) =>
          (for
            // FIXME This is pretty fragile, could use a better way to pass config to the integrations.
            upsName <- connection("upsName").as[String]
            upsHost <- connection("host").as[String]
          yield s"$upsName@$upsHost").toSeq.flatMap { ups =>
            readings.map {
              case ReadingConfig(variable, name, averaging, widgetConfig) =>
                // FIXME It should be left to the widget config.
                val unit = variable match {
                  case "battery.charge" | "ups.load" => "%"
                  case "battery.voltage" | "input.voltage" | "output.voltage" => "V"
                  case "battery.runtime" => "s"
                  case _ => ""
                }
                ObservableReadingConfig(
                  // TODO Could actually populate the model name and type using the UPS provided values.
                  ObservableReading("nut", "ups", name, nutCli.fetch(ups, variable), unit, 0, 100, widgetConfig),
                  samplingInterval.toInt.millis,
                  averaging.toInt
                )
            }
          }
        case ConfigSensorsInner(file, true, samplingInterval, _, readings) if "/(proc|sys/class)/.+".r.matches(file) =>
          readings.map {
            case ReadingConfig(valueName, name, averaging, widgetConfig) =>
              val unit = file match {
                // FIXME It should be left to the widget config.
                case "/proc/loadavg" => ""
                case "/proc/uptime" => "s"
                case "/proc/stat" => ""
                case "/proc/meminfo" => "kB"
                case f if "/sys/class/thermal/.+".r.matches(f) => "°C"
                case f if "/sys/class/net/.+".r.matches(f) => "kBps" // FIXME This only makes sense for transfers.
                case _ => ""
              }
              ObservableReadingConfig(
                ObservableReading("proc-fs", file, name, procFs.get(file, valueName), unit, 0, Double.MaxValue, widgetConfig),
                samplingInterval.toInt.millis,
                averaging.toInt
                )
          }
        case _ => Nil
      }

  private[service] class NodeImpl(config: Config, builder: ReadingsBuilder) extends Node:

    protected val readingsConf: List[ObservableReadingConfig] = builder.build

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
