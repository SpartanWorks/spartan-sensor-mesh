import * as preact from "preact";
import { Gauge, normalize } from "./gauge";

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

interface JetGaugeProps {
  value: number;
  variance?: number;
  min: number;
  max: number;
  errorTooltip: string;
  isError: boolean;
  children: any;
}

export const JetGauge = (props: JetGaugeProps) => {
  const val = normalize(props.value, props.min, props.max);
  return (
    <Gauge progress={val * 360}
           uncertainty={Math.sqrt(props.variance || 0.0) / (props.max - props.min) * 360}
           color={jet(val)}
           isError={props.isError}
           errorTooltip={props.errorTooltip}>
      {props.children}
    </Gauge>
  );
};
