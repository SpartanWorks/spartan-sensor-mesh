import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "./unsupported.css";

interface Props {
  data: SensorData;
}

export const UnsupportedSensor = (props: Props) => (
  <div className={styles.wrapper}>
    <div className={styles.sadPanda}>?</div>
    <div title={"Sensor type " + props.data.type + " is not supported yet."} className={styles.error}/>
  </div>
);
