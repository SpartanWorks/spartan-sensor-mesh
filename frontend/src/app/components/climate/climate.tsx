import * as preact from "preact";
import * as styles from "./climate.css";

interface Props {
  humidity: number;
  temperature: number;
}

export class ClimateWidget extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.wrapper}>
        <div className={styles.humidity}>Humidity: {this.props.humidity}</div>
        <div className={styles.temperature}>Temperature: {this.props.temperature}</div>
      </div>
    );
  }
}
