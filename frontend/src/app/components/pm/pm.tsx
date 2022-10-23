import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { Label } from "../gauge/label";
import { PMGauge } from "../gauge/pm";

interface Props {
  data: SensorData;
  min: number;
  max: number;
}

export const PM = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <PMGauge value={props.data.reading.mean}
               variance={props.data.reading.variance}
               min={props.min}
               max={props.max}
               isError={props.data.status === "error"}
               errorTooltip="Sensor is not responding."
               pmType={props.data.type}>
        <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
               value={Number(props.data.reading.mean.toFixed(1))}
               unit="μg/m³"
               tooltip={"Averaged from last " + props.data.reading.samples + " readings."}/>
      </PMGauge>
    </div>
  </div>
);
