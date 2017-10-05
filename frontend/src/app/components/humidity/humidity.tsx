import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Gauge } from "../gauge/gauge";
import { Label } from "../gauge/label";

interface Props {
  data: SensorData;
}

export const Humidity = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <Gauge progress={props.data.reading.mean * 3.6}
             uncertainty={Math.sqrt(props.data.reading.variance) * 3.6}
             color="dodgerblue"
             isError={props.data.status === "error"}
             errorTooltip="Sensor is not responding.">
        <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
               value={Math.round(props.data.reading.mean)}
               unit="%"
               tooltip={"Averaged from last " + props.data.reading.samples + " readings."}/>
      </Gauge>
    </div>
  </div>
);
