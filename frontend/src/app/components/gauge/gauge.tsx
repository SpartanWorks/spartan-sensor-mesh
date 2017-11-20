import * as preact from "preact";
import * as error from "../../styles/error.css";
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
    "border-color": props.color,
    "visibility": props.stacked ? "hidden" : "visible"
  };

  return (
    <div className={(props.stacked ? styles.stackBottom : styles.stackTop) + " " + styles.gauge + " " + moreLess}>
      <div className={styles.left + " " + styles.progressBar} style={leftStyle}></div>
      <div className={styles.right + " " + styles.progressBar} style={rightStyle}></div>
    </div>
  );
};

function drawLines(props: GaugeProps) {
  if (props.uncertainty === undefined) {
    return (
      <Line color={props.color} progress={props.progress}/>
    );
  } else if (props.uncertainty <= 90) {
    return (
      <div className={styles.stack}>
        <Line color={props.color} progress={props.progress} stacked/>
        <Line color="rgba(0, 0, 0, 0.1)" progress={Math.min(360, props.progress + props.uncertainty)} stacked/>
        <Line color={props.color} progress={Math.max(0, props.progress - props.uncertainty)}/>
      </div>
    );
  } else {
    return (
      // FIXME This could use some extra error icon indication.
      <div className={styles.stack}>
        <Line color={props.color} progress={props.progress}/>
        <Line color="rgba(0, 0, 0, 0.1)" progress={360}/>
      </div>
    );
  }
}

interface GaugeProps {
  color: string;
  progress: number;
  uncertainty?: number;
  isError: boolean;
  errorTooltip: string;
  children?: Array<preact.Component<any, any>>;
}

export const Gauge = (props: GaugeProps) => (
  <div className={styles.wrapper + " " + styles.aliasingFix}>
    <div className={styles.container + " " + error.grayOut + " " + (props.isError ? error.triggered : "")}>
      <div className={styles.shadow}/>
        {drawLines(props)}
      <div className={styles.label}>
        {props.children}
      </div>
    </div>
    <div title={props.errorTooltip} className={error.error + " " + (props.isError ? error.triggered : "")}/>
  </div>
);
