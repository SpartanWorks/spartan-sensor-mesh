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
      <Reading value={props.humidity} unit="%"/>
    </div>
    <div className={styles.temperature}>
      <Reading value={props.temperature} unit="°C"/>
    </div>
  </div>
);
