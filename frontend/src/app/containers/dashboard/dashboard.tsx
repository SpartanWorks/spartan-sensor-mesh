import { observer } from "mobx-preact";
import * as preact from "preact";
import { CO2 } from "../../components/co2/co2";
import { Humidity } from "../../components/humidity/humidity";
import { PM } from "../../components/pm/pm";
import { Pressure } from "../../components/pressure/pressure";
import { iconCogs, RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { Temperature } from "../../components/temperature/temperature";
import { UnsupportedSensor } from "../../components/unsupported/unsupported";
import { VOC } from "../../components/voc/voc";
import { DeviceData, SensorData } from "../../services/device";
import { DashboardStore } from "../../store/dashboard";
import * as styles from "./dashboard.css";

interface Props {
  store: DashboardStore;
}

function renderSensor(data: SensorData) {
  switch (data.type) {
  case "temperature":
    return <Temperature data={data} min={11} max={37}/>;
  case "humidity":
    return <Humidity data={data}/>;
  case "pressure":
    return <Pressure data={data} min={95000} max={105000}/>;
  case "pm2.5":
    return <PM data={data} min={0} max={250}/>;
  case "pm10":
    return <PM data={data} min={0} max={500}/>;
  case "co2":
    return <CO2 data={data} min={0} max={2000}/>;
  case "voc":
    return <VOC data={data} min={0} max={2000}/>;
  default:
    return <UnsupportedSensor data={data}/>;
  }
}

function combineSensors(a: SensorData, b: SensorData): SensorData[] {
  if (a.status === "error" || b.status === "error" || a.status !== b.status || a.reading.unit !== b.reading.unit) {
    return [a, b];
  }

  const total = a.reading.stats.variance + b.reading.stats.variance;
  const aWeight = total === 0 ? 0 : (1 - a.reading.stats.variance / total);
  const bWeight = total === 0 ? 0 : (1 - b.reading.stats.variance / total);

  return [
    {
      type: a.type,
      name: a.name,
      model: "combined",
      status: a.status,
      errors: a.errors + b.errors,
      lastError: "",
      measurements: a.measurements + b.measurements,
      reading: {
        unit: a.reading.unit,
        value: aWeight * a.reading.value + bWeight * b.reading.value,
        range: {
          // NOTE Combined sensors have a reduced range.
          minimum: Math.max(a.reading.range.minimum, b.reading.range.minimum),
          maximum: Math.min(a.reading.range.maximum, b.reading.range.maximum),
        },
        stats: {
          mean: aWeight * a.reading.stats.mean + bWeight * b.reading.stats.mean,
          variance: aWeight * a.reading.stats.variance + bWeight * b.reading.stats.variance, // Uh oh :S
          samples: a.reading.stats.samples + b.reading.stats.samples,
          // NOTE This are likely bogous values.
          maximum: Math.max(a.reading.stats.maximum, b.reading.stats.maximum),
          minimum: Math.min(a.reading.stats.minimum, b.reading.stats.minimum),
        }
      },
    }
  ];
}

function reduceSensors(group: SensorData[]): SensorData[] {
  if (group.length < 2) {
    return group;
  } else {
    let acc = group[0];
    for (let i = 1; i < group.length; i++) {
      const result = combineSensors(acc, group[i]);
      if (result.length !== 1) {
        return result.concat(group.slice(i + 1));
      } else {
        acc = result[0];
      }
    }
    return [acc];
  }
}

function renderSensors(device: DeviceData) {
  const deviceTag = device.group + " / " + device.name;

  const grouped = device.sensors.reduce((groups, s) => {
    const group = s.type + "/" + s.name;
    if (groups.has(group)) {
      groups.get(group)?.push(s);
    } else {
      groups.set(group, [s]);
    }
    return groups;
  }, new Map<string, SensorData[]>());

  const recombined = Array.from(grouped.values())
    .map(reduceSensors)
    .reduce((acc, sensors) => acc.concat(sensors), [] as SensorData[])
    .sort((a, b) => (a.type > b.type ? 1 : -1));

  return (
    <div className={styles.deviceWrapper}>
      <div className={styles.deviceTag}>{ deviceTag }</div>
      { recombined.map(renderSensor) }
    </div>
  );
}

@observer
export class Dashboard extends preact.Component<Props, {}> {
  render() {
    return (
      <div className={styles.mainWrapper}>
        <div className={styles.displayWrapper}>
          { !this.props.store.dataLoaded ? <Spinner/> : this.props.store.data.map(renderSensors) }
        </div>
        <RedirectButton to={"/config"} icon={iconCogs} tooltip="Change configuration parameters."/>
      </div>
    );
  }
}
