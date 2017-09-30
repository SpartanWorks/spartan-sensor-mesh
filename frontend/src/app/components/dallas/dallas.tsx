import * as preact from "preact";
import { SensorData, SensorReading } from "../../services/device";
import { JetGauge } from "../gauge/jet";
import { Label } from "../gauge/label";
import * as styles from "../../styles/widget.css";

interface ReadingProps {
  reading: SensorReading;
  name: string;
  min: number;
  max: number;
  isError: boolean;
  errorTooltip: string;
}

const Reading = (props: ReadingProps) => (
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

interface Props {
  data: SensorData;
  minTemperature: number;
  maxTemperature: number;
}

export const DallasTemperatureSensor = (props: Props) => (
  <div className={styles.widgetWrapper}>
    {
      Object.keys(props.data.readings).map((r) => (
        <Reading reading={props.data.readings[r]}
                 name={r}
                 min={props.minTemperature}
                 max={props.maxTemperature}
                 isError={props.data.status === "error"}
                 errorTooltip="Sensor is not responding."/>
      ))
    }
  </div>
);
