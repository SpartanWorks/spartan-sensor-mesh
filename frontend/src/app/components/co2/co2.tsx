import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "../gauge/color";
import { SensorLabel } from "../gauge/label";

interface Props {
  data: SensorReading;
  min: number;
  max: number;
}

export const CO2 = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <ColorGauge value={props.data.value.stats.mean}
                  min={props.min}
                  max={props.max}
                  variance={props.data.value.stats.variance}
                  color="#616075"
                  isError={props.data.status === "error"}
                  errorTooltip={props.data.lastError}>
        <SensorLabel data={props.data} rounding={0} />
      </ColorGauge>
    </div>
  </div>
);
