import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "../gauge/color";
import { SensorLabel } from "../gauge/label";

interface Props {
  data: SensorReading;
}

export const Humidity = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <ColorGauge value={props.data.value.stats.mean}
                  min={0}
                  max={100}
                  variance={props.data.value.stats.variance}
                  color="dodgerblue"
                  isError={props.data.status === "error"}
                  errorTooltip={props.data.lastError}>
        <SensorLabel data={props.data} rounding={0} />
      </ColorGauge>
    </div>
  </div>
);
