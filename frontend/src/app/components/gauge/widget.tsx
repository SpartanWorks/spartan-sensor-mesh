import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "./color";
import { JetGauge } from "./jet";
import { SensorLabel } from "./label";

interface Props {
  data: SensorReading;
  min?: number;
  max?: number;
  color?: "jet" | string;
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

  if(props.color === "jet") {
    gauge = (
      <JetGauge {...ps}>
        <SensorLabel data={props.data} rounding={props.rounding ?? 1} />
      </JetGauge>
    );
  } else {
    gauge = (
      <ColorGauge color={props.color ?? "gray"} {...ps}>
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
}
