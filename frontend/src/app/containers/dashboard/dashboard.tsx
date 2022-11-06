import { observer } from "mobx-preact";
import * as preact from "preact";
import { GaugeWidget } from "../../components/gauge/widget";
import { Pressure } from "../../components/pressure/pressure";
import { iconCogs, RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { UnsupportedSensor } from "../../components/unsupported/unsupported";
import { DeviceData, SensorReading } from "../../services/device";
import { DashboardStore } from "../../store/dashboard";
import * as styles from "./dashboard.css";

interface Props {
  store: DashboardStore;
}

function renderSensor(data: SensorReading) {
  const t = data.config.type ?? data.type;

  switch (t) {
  case "pressure":
    return <Pressure data={data} {...data.config}/>;

  case "gauge":
    return <GaugeWidget data={data} {...data.config}/>;

  default:
    return <UnsupportedSensor data={data}/>;
  }
}

function combineReadings(a: SensorReading, b: SensorReading): SensorReading[] {
  if (a.status === "error" || b.status === "error"
    || a.status !== b.status
    || a.value.unit !== b.value.unit
    || JSON.stringify(a.config) !== JSON.stringify(b.config)) {
    return [a, b];
  }

  const total = a.value.stats.variance + b.value.stats.variance;
  const aWeight = total === 0 ? 0 : (1 - a.value.stats.variance / total);
  const bWeight = total === 0 ? 0 : (1 - b.value.stats.variance / total);

  return [
    {
      ...a,
      model: "combined",
      errors: a.errors + b.errors,
      lastError: "",
      measurements: a.measurements + b.measurements,
      value: {
        unit: a.value.unit,
        last: aWeight * a.value.last + bWeight * b.value.last,
        range: {
          // NOTE Combined sensors have a reduced range.
          minimum: Math.max(a.value.range.minimum, b.value.range.minimum),
          maximum: Math.min(a.value.range.maximum, b.value.range.maximum),
        },
        stats: {
          mean: aWeight * a.value.stats.mean + bWeight * b.value.stats.mean,
          variance: aWeight * a.value.stats.variance + bWeight * b.value.stats.variance, // Uh oh :S
          samples: a.value.stats.samples + b.value.stats.samples,
          // NOTE This are likely bogous values.
          maximum: Math.max(a.value.stats.maximum, b.value.stats.maximum),
          minimum: Math.min(a.value.stats.minimum, b.value.stats.minimum),
        }
      },
    }
  ];
}

function reduceReadings(group: SensorReading[]): SensorReading[] {
  if (group.length < 2) {
    return group;
  } else {
    let acc = group[0];
    for (let i = 1; i < group.length; i++) {
      const result = combineReadings(acc, group[i]);
      if (result.length !== 1) {
        return result.concat(group.slice(i + 1));
      } else {
        acc = result[0];
      }
    }
    return [acc];
  }
}

function deviceTag(device: DeviceData): string {
  return device.group + " / " + device.name;
}

function renderReadings(device: DeviceData) {
  const grouped = device.readings.reduce((groups, s) => {
    const group = s.type + "/" + s.name;
    if (groups.has(group)) {
      groups.get(group)?.push(s);
    } else {
      groups.set(group, [s]);
    }
    return groups;
  }, new Map<string, SensorReading[]>());

  const recombined = Array.from(grouped.values())
    .map(reduceReadings)
    .reduce((acc, sensors) => acc.concat(sensors), [] as SensorReading[])
    .sort((a, b) => (a.type > b.type ? 1 : -1));

  return (
    <div className={styles.deviceWrapper}>
      <div className={styles.deviceTag}>{ deviceTag(device) }</div>
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
          {
            !this.props.store.dataLoaded
            ? <Spinner/>
            : this.props.store.data.sort((a, b) => (deviceTag(a) > deviceTag(b) ? 1 : -1)).map(renderReadings)
          }
        </div>
        <RedirectButton to={"/config"} icon={iconCogs} tooltip="Change configuration parameters."/>
      </div>
    );
  }
}
