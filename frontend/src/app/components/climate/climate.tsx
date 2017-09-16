import * as preact from "preact";
import * as styles from "./climate.css";
import { Reading } from "../reading/reading";

interface Props {
  humidity: number;
  temperature: number;
}

export const ClimateWidget = (props: Props) => (
  <div className={styles.wrapper}>
    <div className={styles.humidity}>
      <Reading label={"" + Math.round(props.humidity) + "%"}
               progress={Math.round(props.humidity * 3.6)}
               color="blue"/>
    </div>
    <div className={styles.temperature}>
      <Reading label={"" + Math.round(props.temperature) + "°C"}
               progress={360}
               color="red"/>
    </div>
  </div>
);
