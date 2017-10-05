import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Gauge } from "../gauge/gauge";
import { Label } from "../gauge/label";

interface Props {
  reading: SensorReading;
  name: string;
  isError: boolean;
}

export const Humidity = (props: Props) => (
  <div className={styles.readingWrapper}>
    <Gauge progress={props.reading.mean * 3.6}
           uncertainty={Math.sqrt(props.reading.variance) * 3.6}
           color="dodgerblue"
           isError={props.isError}
           errorTooltip="Sensor is not responding.">
      <Label name={props.name}
             value={Math.round(props.reading.mean)}
             unit="%"
             tooltip={"Averaged from last " + props.reading.samples + " readings."}/>
    </Gauge>
  </div>
);
