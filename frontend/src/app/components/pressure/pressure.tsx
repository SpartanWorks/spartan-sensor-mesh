import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { JetGauge } from "../gauge/jet";
import { Label, readingTooltip } from "../gauge/label";

interface Props {
  data: SensorReading;
  min: number;
  max: number;
}

export const Pressure = (props: Props) => {
  // NOTE Scaling to hPa for more down-to-earth display.
  const scaledValue = Math.round(props.data.value.stats.mean / 100.0);
  return (
    <div className={styles.widgetWrapper}>
      <div className={styles.readingWrapper}>
        <JetGauge value={props.data.value.stats.mean}
                  variance={props.data.value.stats.variance}
                  min={props.min}
                  max={props.max}
                  isError={props.data.status === "error"}
                  errorTooltip={props.data.lastError}>
          <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
                 value={scaledValue}
                 unit={"h" + props.data.value.unit}
                 tooltip={readingTooltip({
                   ...props.data.value,
                   stats: {
                     ...props.data.value.stats,
                     mean: scaledValue,
                     variance: props.data.value.stats.variance / 10000
                   }
                 }, 0)}/>
        </JetGauge>
      </div>
    </div>
  );
};
