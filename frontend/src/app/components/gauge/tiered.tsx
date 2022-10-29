import * as preact from "preact";
import { ColorGauge } from "./color";

export interface Tier {
  min: number;
  max: number;
  color: string;
}

function identifyTier(value: number, tiers: Tier[], defaultTier: Tier): Tier {
  const needle = tiers.find((tier) => {
    if (tier.min <= value && tier.max > value) { return true; }
    else { return false; }
  });
  return needle || defaultTier;
}

interface TieredGaugeProps {
  value: number;
  variance?: number;
  tiers: Tier[];
  defaultTier: Tier;
  errorTooltip: string;
  isError: boolean;
  children: any;
}

export const TieredGauge = (props: TieredGaugeProps) => {
  const tier = identifyTier(props.value, props.tiers, props.defaultTier);
  return (
    <ColorGauge value={props.value}
                variance={props.variance}
                min={Math.min(tier.min, props.value)}
                max={Math.max(tier.max, props.value)}
                color={tier.color}
                isError={props.isError}
                errorTooltip={props.errorTooltip}>
      {props.children}
    </ColorGauge>
  );
};
