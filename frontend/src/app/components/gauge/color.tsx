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
  const val = normalize(props.value, props.min, props.max);
  return (
    <Gauge progress={val * 360}
           uncertainty={Math.sqrt(props.variance || 0.0) / (props.max - props.min) * 360}
           color={props.color}
           isError={props.isError}
           errorTooltip={props.errorTooltip}>
      {props.children}
    </Gauge>
  );
};
