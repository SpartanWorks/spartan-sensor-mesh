import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Gauge } from "../gauge/gauge";
import { Label } from "../gauge/label";
import { Temperature } from "../temperature/temperature";

interface Props {
  data: SensorData;
  minTemperature: number;
  maxTemperature: number;
}

export const DHTSensor = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <Gauge progress={props.data.readings.humidity.mean * 3.6}
             uncertainty={Math.sqrt(props.data.readings.humidity.variance) * 3.6}
             color="dodgerblue"
             isError={props.data.status === "error"}
             errorTooltip="Sensor is not responding.">
        <Label name="Humidity"
               value={Math.round(props.data.readings.humidity.mean)}
               unit="%"
               tooltip={"Averaged from last " + props.data.readings.humidity.samples + " readings."}/>
      </Gauge>
    </div>
    <Temperature reading={props.data.readings.temperature}
                 name="Temperature"
                 min={props.minTemperature}
                 max={props.maxTemperature}
                 isError={props.data.status === "error"}/>
  </div>
);
