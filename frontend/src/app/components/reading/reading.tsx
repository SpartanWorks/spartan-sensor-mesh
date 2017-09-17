import * as preact from "preact";
import * as styles from "./reading.css";

interface Props {
  label: string;
  color: string;
  progress: number;
}

export const Reading = (props: Props) => {
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
    <div className={styles.wrapper + " " + moreLess}>
      <span className={styles.label}>{props.label}</span>
      <div className={styles.gauge}>
        <div className={styles.left + " " + styles.progressBar} style={leftStyle}></div>
        <div className={styles.right + " " + styles.progressBar} style={rightStyle}></div>
      </div>
      <div className={styles.shadow}/>
    </div>
  );
};
