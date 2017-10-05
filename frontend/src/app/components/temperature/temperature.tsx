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
  errorTooltip: string;
}

export const Temperature = (props: Props) => (
  <div className={styles.readingWrapper}>
    <JetGauge value={props.reading.mean}
              variance={props.reading.variance}
              min={props.min}
              max={props.max}
              isError={props.isError}
              errorTooltip={props.errorTooltip}>
      <Label name={props.name[0].toUpperCase() + props.name.substring(1)}
             value={Math.round(props.reading.mean)}
             unit="°C"
             tooltip={"Averaged from last " + props.reading.samples + " readings."}/>
    </JetGauge>
  </div>
);
