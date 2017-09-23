import * as preact from "preact";
import * as styles from "./reading.css";

interface GaugeProps {
  color: string;
  progress: number;
}

const Gauge = (props: GaugeProps) => {
  const moreLess = (props.progress > 180) ? styles.progressMore : styles.progressLess;
  const leftStyle = {
    "border-color": props.color,
    "transform": "rotate(" + props.progress + "deg)",
    "-webkit-transform": "rotate(" + props.progress + "deg)",
    "-moz-transform": "rotate(" + props.progress + "deg)",
    "-ms-transform": "rotate(" + props.progress + "deg)"
  };
  const rightStyle = {
    "border-color": props.color
  };

  return (
    <div className={moreLess + " " + styles.gauge}>
      <div className={styles.left + " " + styles.progressBar} style={leftStyle}></div>
      <div className={styles.right + " " + styles.progressBar} style={rightStyle}></div>
    </div>
  );
}

interface Props {
  color: string;
  progress: number;
  isError: boolean;
  children?: Array<preact.Component<any, any>>;
}

export const Reading = (props: Props) => (
  <div className={styles.wrapper}>
    <div className={styles.shadow}/>
    <Gauge color={props.color} progress={props.progress}/>
    <div className={styles.label}>
      {props.children}
    </div>
    <div title="Sensor is not responding." className={props.isError ? styles.error : styles.hidden}/>
  </div>
);
