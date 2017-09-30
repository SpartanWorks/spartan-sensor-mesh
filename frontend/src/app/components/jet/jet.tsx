import * as preact from "preact";
import { Reading } from "../reading/reading";

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

function normalize(val: number, min: number, max: number): number {
  return (clamp(val, min, max) - min) / (max - min);
}

interface Props {
  colorVal: number;
  colorMin: number;
  colorMax: number;
  progress: number;
  uncertainty?: number;
  isError: boolean;
  children?: Array<preact.Component<any, any>>;
}

export const JetReading = (props: Props) => (
  <Reading progress={props.progress}
           uncertainty={props.uncertainty}
           color={jet(normalize(props.colorVal, props.colorMin, props.colorMax))}
           isError={props.isError}>
    {props.children}
  </Reading>
);
