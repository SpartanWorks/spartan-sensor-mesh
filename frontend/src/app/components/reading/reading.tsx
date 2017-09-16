import * as preact from "preact";
import * as styles from "./reading.css";

interface Props {
  value: number;
  unit: string;
}

export const Reading = (props: Props) => (
  <div className={styles.wrapper}>
    <span className={styles.value}>{Math.round(props.value)}</span>
    <span className={styles.unit}>{props.unit}</span>
  </div>
);
