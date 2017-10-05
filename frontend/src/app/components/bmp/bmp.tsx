import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Pressure } from "../pressure/pressure";
import { Temperature } from "../temperature/temperature";

interface Props {
  data: SensorData;
  minPressure: number;
  maxPressure: number;
  minTemperature: number;
  maxTemperature: number;
}

export const BMPSensor = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <Pressure reading={props.data.readings.pressure}
              name="Pressure"
              min={props.minPressure}
              max={props.maxPressure}
              isError={props.data.status === "error"}/>
    <Temperature reading={props.data.readings.temperature}
                 name="Temperature"
                 min={props.minTemperature}
                 max={props.maxTemperature}
                 isError={props.data.status === "error"}/>
  </div>
);
