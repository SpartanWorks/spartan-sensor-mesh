import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "./color";
import { SensorLabel } from "./label";

interface Props {
  data: SensorReading;
  min?: number;
  max?: number;
  color?: string;
  rounding?: number;
}

export const GaugeWidget = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <ColorGauge value={props.data.value.stats.mean}
                  min={props.min ?? props.data.value.range.minimum}
                  max={props.max ?? props.data.value.range.maximum}
                  variance={props.data.value.stats.variance}
                  color={props.color ?? "gray"}
                  isError={props.data.status === "error"}
                  errorTooltip={props.data.lastError}>
        <SensorLabel data={props.data} rounding={props.rounding ?? 2} />
      </ColorGauge>
    </div>
  </div>
);
