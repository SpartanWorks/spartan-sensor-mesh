import * as preact from "preact";
import { route } from "preact-router";
import * as styles from "./redirect.css";

interface Props {
  to: string;
  icon: string;
  tooltip?: string;
}

export const iconCogs = styles.iconCogs;
export const iconChart = styles.iconChart;

export const RedirectButton = (props: Props) => (
  <div className={styles.wrapper}>
    <div title={props.tooltip} className={styles.button + " " + props.icon} onClick={() => route(props.to)}/>
  </div>
);
