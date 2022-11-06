import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { JetGauge } from "../gauge/jet";
import { SensorLabel } from "../gauge/label";

interface Props {
  data: SensorReading;
  min: number;
  max: number;
}

export const Temperature = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <JetGauge value={props.data.value.stats.mean}
                variance={props.data.value.stats.variance}
                min={props.min}
                max={props.max}
                isError={props.data.status === "error"}
                errorTooltip={props.data.lastError}>
        <SensorLabel data={props.data}/>
      </JetGauge>
    </div>
  </div>
);
