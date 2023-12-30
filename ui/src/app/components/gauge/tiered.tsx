import * as preact from "preact";
import { ColorGauge, ColorBand } from "./color";

function identifyTier(value: number, tiers: ColorBand[], defaultTier: ColorBand): ColorBand {
  const needle = tiers.find((tier) => (tier.min <= value && tier.max > value));
  return needle || defaultTier;
}

interface TieredGaugeProps {
  value: number;
  variance?: number;
  tiers: ColorBand[];
  defaultTier: ColorBand;
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
                defaultColor={props.defaultTier.color}
                isError={props.isError}
                errorTooltip={props.errorTooltip}>
      {props.children}
    </ColorGauge>
  );
};
