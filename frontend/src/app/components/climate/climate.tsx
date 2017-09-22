import * as preact from "preact";
import { SensorData } from "../../services/sensor";
import { Reading } from "../reading/reading";
import * as styles from "./climate.css";

interface Props {
  data: SensorData;
  minTemperature: number;
  maxTemperature: number;
}

function interpolate(val: number, y0: number, x0: number, y1: number, x1: number): number {
  return (val - x0) * (y1 - y0) / (x1 - x0) + y0;
}

function jetBase(val: number): number {
  if (val <= 0.125) {
    return 0.0;
  } else if (val <= 0.375) {
    return interpolate(val, 0.0, 0.125, 1.0, 0.375);
  } else if (val <= 0.625) {
    return 1.0;
  } else if (val <= 0.875) {
    return interpolate(val, 1.0, 0.625, 0.0, 0.875);
  } else {
    return 0.0;
  }
}

function hexify(val: number, padding: number): string {
  let hex = Math.round(val * 255).toString(16);
  while (hex.length < padding) {
    hex = "0" + hex;
  }
  return hex;
}

function jet(base: number): string {
  return "#" + hexify(jetBase(base - 0.25), 2) + hexify(jetBase(base), 2) + hexify(jetBase(base + 0.25), 2);
}

function clamp(val: number, min: number, max: number): number {
  return Math.max(min, Math.min(val, max));
}

function valueWithin(val: number, min: number, max: number): number {
  return (clamp(val, min, max) - min) / (max - min);
}

interface LabelProps {
  label: string;
  value: number;
  unit: string;
  tooltip: string;
}

export const Label = (props: LabelProps) => (
  <div className={styles.labelWrapper}>
    <span className={styles.dummy}>{props.label}</span>
    <div title={props.tooltip} className={styles.reading}>
      <span className={styles.value}>{"" + props.value}</span>
      <span className={styles.unit}>{props.unit}</span>
    </div>
    <span className={styles.label}>{props.label}</span>
  </div>
);

function getTooltip(window?: number) {
  return window ? ("Averaged from " + window + " readings.") : "Exact reading.";
}

export const ClimateWidget = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <Reading progress={props.data.humidity.mean * 3.6}
               color="dodgerblue"
               isError={props.data.status === "error"}>
      <Label label="Humidity"
             value={Math.round(props.data.humidity.mean)}
             unit="%"
             tooltip={getTooltip(props.data.humidity.window)}/>
      </Reading>
    </div>
    <div className={styles.readingWrapper}>
      <Reading progress={360}
               color={jet(valueWithin(props.data.temperature.mean, props.minTemperature, props.maxTemperature))}
               isError={props.data.status === "error"}>
        <Label label="Temperature"
               value={Math.round(props.data.temperature.mean)}
               unit="°C"
               tooltip={getTooltip(props.data.temperature.window)}/>
      </Reading>
    </div>
  </div>
);
