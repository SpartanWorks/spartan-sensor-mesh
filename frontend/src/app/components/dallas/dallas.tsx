import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Temperature } from "../temperature/temperature";

interface Props {
  data: SensorData;
  minTemperature: number;
  maxTemperature: number;
}

export const DallasTemperatureSensor = (props: Props) => (
  <div className={styles.widgetWrapper}>
    {
      Object.keys(props.data.readings).map((r) => (
        <Temperature reading={props.data.readings[r]}
                     name={r}
                     min={props.minTemperature}
                     max={props.maxTemperature}
                     isError={props.data.status === "error"}
                     errorTooltip="Sensor is not responding."/>
      ))
    }
  </div>
);
