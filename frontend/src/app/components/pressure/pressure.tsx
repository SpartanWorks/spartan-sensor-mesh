import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { JetGauge } from "../gauge/jet";
import { Label } from "../gauge/label";

interface Props {
  reading: SensorReading;
  name: string;
  min: number;
  max: number;
  isError: boolean;
}

export const Pressure = (props: Props) => (
  <div className={styles.readingWrapper}>
    <JetGauge value={props.reading.mean}
              variance={props.reading.variance}
              min={props.min}
              max={props.max}
              isError={props.isError}
              errorTooltip="Sensor is not responding.">
      <Label name={props.name}
             value={Math.round(props.reading.mean / 100.0)}
             unit="hPa"
             tooltip={"Averaged from last " + props.reading.samples + " readings."}/>
    </JetGauge>
  </div>
);
