import * as preact from "preact";
import { SensorReading } from "../../services/device";
import * as error from "../../styles/error.css";
import { readingWrapper, widgetWrapper } from "../../styles/widget.css";
import * as styles from "./unsupported.css";

interface Props {
  data: SensorReading;
}

export const UnsupportedSensor = (props: Props) => (
  <div className={widgetWrapper}>
    <div className={readingWrapper + " " + styles.widget}>
      <div className={[styles.sadPanda, error.grayOut, error.triggered].join(" ")}>?</div>
      <div title={"Sensor type " + props.data.type + " is not supported yet."}
           className={error.error + " " + error.triggered}/>
    </div>
  </div>
);
