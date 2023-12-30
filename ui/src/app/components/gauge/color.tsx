import * as preact from "preact";
import { Gauge, normalize } from "./gauge";

export interface ColorBand {
  min: number;
  max: number;
  color: string;
}

interface ColorGaugeProps {
  value: number;
  variance?: number;
  min: number;
  max: number;
  errorTooltip: string;
  isError: boolean;
  color: string | ColorBand[];
  defaultColor: string;
  children: any;
}

function identifyColor(value: number, color: string | ColorBand[], defaultColor: string): string {
  if (typeof color === "string") {
    return color;
  }

  const needle = (color as ColorBand[]).find((band) => (band.min <= value && band.max > value));

  return needle?.color || defaultColor;
}

export const ColorGauge = (props: ColorGaugeProps) => {
  const max = Math.max(props.max, props.value);
  const min = Math.min(props.min, props.value);
  const val = normalize(props.value, min, max);
  const color = identifyColor(props.value, props.color, props.defaultColor);
  return (
    <Gauge progress={val * 360}
           uncertainty={Math.sqrt(props.variance || 0.0) / (max - min) * 360}
           color={color}
           isError={props.isError}
           errorTooltip={props.errorTooltip}>
      {props.children}
    </Gauge>
  );
};
