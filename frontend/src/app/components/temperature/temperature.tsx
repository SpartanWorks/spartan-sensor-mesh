import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { JetGauge } from "../gauge/jet";
import { Label } from "../gauge/label";

interface Props {
  data: SensorData;
  min: number;
  max: number;
}

export const Temperature = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <JetGauge value={props.data.reading.mean}
                variance={props.data.reading.variance}
                min={props.min}
                max={props.max}
                isError={props.data.status === "error"}
                errorTooltip="Sensor is not responding.">
        <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
               value={Number(props.data.reading.mean.toFixed(1))}
               unit="°C"
               tooltip={"Averaged from last " + props.data.reading.samples + " readings."}/>
      </JetGauge>
    </div>
  </div>
);
