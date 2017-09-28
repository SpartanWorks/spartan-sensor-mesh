import * as preact from "preact";
import { error } from "../../styles/error.css";
import * as styles from "./reading.css";

interface GaugeProps {
  color: string;
  progress: number;
  stacked?: boolean;
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
    <div className={(props.stacked ? styles.stackBottom : styles.stackTop) + " " + styles.gauge + " " + moreLess}>
      <div className={styles.left + " " + styles.progressBar} style={leftStyle}></div>
      <div className={styles.right + " " + styles.progressBar} style={rightStyle}></div>
    </div>
  );
};

interface Props {
  color: string;
  progress: number;
  uncertainty?: number;
  isError: boolean;
  children?: Array<preact.Component<any, any>>;
}

export const Reading = (props: Props) => (
  <div className={styles.wrapper + " " + styles.aliasingFix}>
    <div className={styles.shadow}/>
    {
      props.uncertainty ? (
        <div className={styles.stack}>
          <Gauge color={props.color} progress={props.progress} stacked/>
          <Gauge color="rgba(0, 0, 0, 0.1)" progress={Math.min(360, props.progress + props.uncertainty)} stacked/>
          <Gauge color={props.color} progress={Math.max(0, props.progress - props.uncertainty)}/>
        </div>
      ) : (
        <Gauge color={props.color} progress={props.progress}/>
      )
    }
    <div className={styles.label}>
      {props.children}
    </div>
    <div title="Sensor is not responding." className={props.isError ? error : styles.hidden}/>
  </div>
);
