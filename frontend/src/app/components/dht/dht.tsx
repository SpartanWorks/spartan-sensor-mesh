import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Humidity } from "../humidity/humidity";
import { Temperature } from "../temperature/temperature";

interface Props {
  data: SensorData;
  minTemperature: number;
  maxTemperature: number;
}

export const DHTSensor = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <Humidity reading={props.data.readings.humidity}
              name="Humidity"
              isError={props.data.status === "error"}/>
    <Temperature reading={props.data.readings.temperature}
                 name="Temperature"
                 min={props.minTemperature}
                 max={props.maxTemperature}
                 isError={props.data.status === "error"}/>
  </div>
);
