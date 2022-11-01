import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "../gauge/color";
import { SensorLabel } from "../gauge/label";

interface Props {
  data: SensorData;
}

export const Humidity = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <ColorGauge value={props.data.reading.stats.mean}
                  min={0}
                  max={100}
                  variance={props.data.reading.stats.variance}
                  color="dodgerblue"
                  isError={props.data.status === "error"}
                  errorTooltip={props.data.lastError}>
        <SensorLabel data={props.data} rounding={0} />
      </ColorGauge>
    </div>
  </div>
);
