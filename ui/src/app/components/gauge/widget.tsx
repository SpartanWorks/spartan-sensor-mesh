import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorBand, ColorGauge } from "./color";
import { JetGauge } from "./jet";
import { SensorLabel } from "./label";
import { TieredGauge } from "./tiered";

interface Props {
  data: SensorReading;
  min?: number;
  max?: number;
  color?: "jet" | string | ColorBand[];
  tiers?: ColorBand[];
  rounding?: number;
}

export const GaugeWidget = (props: Props) => {
  const ps = {
    value: props.data.value.stats.mean,
    variance: props.data.value.stats.variance,
    min: props.min ?? props.data.value.range.minimum,
    max: props.max ?? props.data.value.range.maximum,
    isError: props.data.status === "error",
    errorTooltip: props.data.lastError,
  };

  let gauge;

  if (!!props.tiers) {
    const def = {
      min: ps.min,
      max: ps.max,
      color: props.tiers.at(-1)?.color ?? props.tiers.at(-1)?.color ?? "gray",
    };
    gauge = (
      <TieredGauge tiers={props.tiers} defaultTier={def} {...ps}>
        <SensorLabel data={props.data} rounding={props.rounding ?? 1} />
      </TieredGauge>
    );
  } else if (props.color === "jet") {
    gauge = (
      <JetGauge {...ps}>
        <SensorLabel data={props.data} rounding={props.rounding ?? 1} />
      </JetGauge>
    );
  } else {
    gauge = (
      <ColorGauge color={props.color || "gray"} defaultColor="gray" {...ps}>
        <SensorLabel data={props.data} rounding={props.rounding ?? 1} />
      </ColorGauge>
    );
  }

  return (
    <div className={styles.widgetWrapper}>
      <div className={styles.readingWrapper}>
        {gauge}
      </div>
   </div>
  );
};
