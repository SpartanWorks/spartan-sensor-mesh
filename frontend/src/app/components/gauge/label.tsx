import * as preact from "preact";
import { SensorData, SensorReading } from "../../services/device";
import * as styles from "./label.css";

interface LabelProps {
  name: string;
  value: number;
  unit: string;
  tooltip: string;
}

export const Label = (props: LabelProps) => {
  const transform = {
    "transform": "scale(" + Math.min(1.0, 5 / (props.value + props.unit).length) + ")"
  };
  return (
    <div className={styles.labelWrapper}>
      <span className={styles.dummy}>{props.name}</span>
      <div title={props.tooltip} className={styles.valueWrapper} style={transform}>
        <span className={styles.value}>{props.value}</span>
        <span className={styles.unit}>{props.unit}</span>
      </div>
      <span title={props.name} className={styles.name}>{props.name}</span>
    </div>
  );
};

interface SensorLabelProps {
  data: SensorData;
  rounding?: number;
}

export function readingTooltip(reading: SensorReading, rounding?: number): string {
  const r = (rounding ?? 1) + 2;
  return (
    "Walking average: " + reading.stats.mean.toFixed(r) + " ± " + Math.sqrt(reading.stats.variance).toFixed(r) +
    "\nNumber of samples: " + reading.stats.samples +
    "\nMaximum: " + reading.stats.maximum.toFixed(r) +
    "\nMinimum: " + reading.stats.minimum.toFixed(r)
  );
}

export const SensorLabel = (props: SensorLabelProps) => {
  return (
    <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
           value={Number(props.data.reading.stats.mean.toFixed(props.rounding ?? 1))}
           unit={props.data.reading.unit}
           tooltip={readingTooltip(props.data.reading, props.rounding)}/>
  );
};
