import * as preact from "preact";
import { Gauge, normalize } from "./gauge";

function pm10Color(value: number): string {
  if (value < 54) { return "#00e400"; }
  if (value < 154) { return "#ff0"; }
  if (value < 254) { return "#ff7e00"; }
  if (value < 354) { return "#f00"; }
  if (value < 424) { return "#8f3f97"; }
  return "#7e0023";
}

function pm25Color(value: number): string {
  if (value < 12) { return "#00e400"; }
  if (value < 35) { return "#ff0"; }
  if (value < 55) { return "#ff7e00"; }
  if (value < 150) { return "#f00"; }
  if (value < 199) { return "#8f3f97"; }
  return "#7e0023";
}

interface PMGaugeProps {
  value: number;
  variance?: number;
  min: number;
  max: number;
  errorTooltip: string;
  isError: boolean;
  pmType: string;
  children: any;
}

export const PMGauge = (props: PMGaugeProps) => {
  const val = normalize(props.value, props.min, props.max);
  return (
    <Gauge progress={val * 360}
           uncertainty={Math.sqrt(props.variance || 0.0) / (props.max - props.min) * 360}
           color={props.pmType === "PM10" ? pm10Color(props.value) : pm25Color(props.value)}
           isError={props.isError}
           errorTooltip={props.errorTooltip}>
      {props.children}
    </Gauge>
  );
};
