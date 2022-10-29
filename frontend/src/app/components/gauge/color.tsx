import * as preact from "preact";
import { Gauge, normalize } from "./gauge";

interface ColorGaugeProps {
  value: number;
  variance?: number;
  min: number;
  max: number;
  errorTooltip: string;
  isError: boolean;
  color: string;
  children: any;
}

export const ColorGauge = (props: ColorGaugeProps) => {
  const max = Math.max(props.max, props.value);
  const min = Math.min(props.min, props.value);
  const val = normalize(props.value, min, max);
  return (
    <Gauge progress={val * 360}
           uncertainty={Math.sqrt(props.variance || 0.0) / (max - min) * 360}
           color={props.color}
           isError={props.isError}
           errorTooltip={props.errorTooltip}>
      {props.children}
    </Gauge>
  );
};
