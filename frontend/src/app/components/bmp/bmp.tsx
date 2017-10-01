import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { JetGauge } from "../gauge/jet";
import { Label } from "../gauge/label";

interface Props {
  data: SensorData;
  minPressure: number;
  maxPressure: number;
  minTemperature: number;
  maxTemperature: number;
}

export const BMPSensor = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <JetGauge value={props.data.readings.pressure.mean}
                variance={props.data.readings.pressure.variance}
                min={props.minPressure}
                max={props.maxPressure}
                isError={props.data.status === "error"}
                errorTooltip="Sensor is not responding.">
        <Label name="Pressure"
               value={Math.round(props.data.readings.pressure.mean)}
               unit="hPa"
               tooltip={"Averaged from last " + props.data.readings.pressure.samples + " readings."}/>
      </JetGauge>
    </div>
    <div className={styles.readingWrapper}>
      <JetGauge value={props.data.readings.temperature.mean}
                variance={props.data.readings.temperature.variance}
                min={props.minTemperature}
                max={props.maxTemperature}
                isError={props.data.status === "error"}
                errorTooltip="Sensor is not responding.">
        <Label name="Temperature"
               value={Math.round(props.data.readings.temperature.mean)}
               unit="°C"
               tooltip={"Averaged from last " + props.data.readings.temperature.samples + " readings."}/>
      </JetGauge>
    </div>
  </div>
);
