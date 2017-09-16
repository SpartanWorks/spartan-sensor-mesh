import * as preact from "preact";
import * as styles from "./climate.css";

interface Props {
  humidity: number;
  temperature: number;
}

export const ClimateWidget = (props: Props) => (
  <div className={styles.wrapper}>
    <div className={styles.humidity}>Humidity: {props.humidity}</div>
    <div className={styles.temperature}>Temperature: {props.temperature}</div>
  </div>
);
