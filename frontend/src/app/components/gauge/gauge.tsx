import * as preact from "preact";
import { error } from "../../styles/error.css";
import * as styles from "./gauge.css";

interface LineProps {
  color: string;
  progress: number;
  stacked?: boolean;
}

const Line = (props: LineProps) => {
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

interface GaugeProps {
  color: string;
  progress: number;
  uncertainty?: number;
  isError: boolean;
  children?: Array<preact.Component<any, any>>;
}

export const Gauge = (props: GaugeProps) => (
  <div className={styles.wrapper + " " + styles.aliasingFix}>
    <div className={styles.shadow}/>
    {
      props.uncertainty ? (
        <div className={styles.stack}>
          <Line color={props.color} progress={props.progress} stacked/>
          <Line color="rgba(0, 0, 0, 0.1)" progress={Math.min(360, props.progress + props.uncertainty)} stacked/>
          <Line color={props.color} progress={Math.max(0, props.progress - props.uncertainty)}/>
        </div>
      ) : (
        <Line color={props.color} progress={props.progress}/>
      )
    }
    <div className={styles.label}>
      {props.children}
    </div>
    <div title="Sensor is not responding." className={props.isError ? error : styles.hidden}/>
  </div>
);

interface LabelProps {
  name: string;
  value: number;
  unit: string;
  tooltip: string;
}

export const Label = (props: LabelProps) => (
  <div className={styles.labelContent}>
    <span className={styles.dummy}>{props.name}</span>
    <div title={props.tooltip} className={styles.reading}>
      <span className={styles.value}>{"" + props.value}</span>
      <span className={styles.unit}>{props.unit}</span>
    </div>
    <span className={styles.labelName}>{props.name}</span>
  </div>
);
