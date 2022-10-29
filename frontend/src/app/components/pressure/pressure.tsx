import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { JetGauge } from "../gauge/jet";
import { Label, readingTooltip } from "../gauge/label";

interface Props {
  data: SensorData;
  min: number;
  max: number;
}

export const Pressure = (props: Props) => {
  // NOTE Scaling to hPa for more down-to-earth display.
  const scaledValue = Math.round(props.data.reading.mean / 100.0);
  return (
    <div className={styles.widgetWrapper}>
      <div className={styles.readingWrapper}>
        <JetGauge value={props.data.reading.mean}
                  variance={props.data.reading.variance}
                  min={props.min}
                  max={props.max}
                  isError={props.data.status === "error"}
                  errorTooltip={props.data.lastError}>
          <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
                 value={scaledValue}
                 unit={"h" + props.data.unit}
                 tooltip={readingTooltip({
                   ...props.data.reading,
                   mean: scaledValue,
                   variance: props.data.reading.variance/10000
                 }, 0)}/>
        </JetGauge>
      </div>
    </div>
  )
};
