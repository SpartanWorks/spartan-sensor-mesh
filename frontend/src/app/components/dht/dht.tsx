import * as preact from "preact";
import { SensorData } from "../../services/device";
import { Gauge, Label } from "../gauge/gauge";
import { JetGauge } from "../gauge/jet";
import * as styles from "./dht.css";

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
             isError={props.data.status === "error"}>
        <Label name="Humidity"
               value={Math.round(props.data.readings.humidity.mean)}
               unit="%"
               tooltip={"Averaged from last " + props.data.readings.humidity.samples + " readings."}/>
      </Gauge>
    </div>
    <div className={styles.readingWrapper}>
      <JetGauge progress={360}
                colorVal={props.data.readings.temperature.mean}
                colorMin={props.minTemperature}
                colorMax={props.maxTemperature}
                isError={props.data.status === "error"}>
        <Label name="Temperature"
               value={Math.round(props.data.readings.temperature.mean)}
               unit="°C"
               tooltip={"Averaged from last " + props.data.readings.temperature.samples + " readings."}/>
      </JetGauge>
    </div>
  </div>
);
