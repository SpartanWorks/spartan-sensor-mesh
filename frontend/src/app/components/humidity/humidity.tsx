import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "../gauge/color";
import { Label } from "../gauge/label";

interface Props {
  data: SensorData;
}

export const Humidity = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <ColorGauge value={props.data.reading.mean}
                  min={0}
                  max={100}
                  variance={props.data.reading.variance}
                  color="dodgerblue"
                  isError={props.data.status === "error"}
                  errorTooltip="Sensor is not responding.">
        <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
               value={Math.round(props.data.reading.mean)}
               unit="%"
               tooltip={"Averaged from last " + props.data.reading.samples + " readings."}/>
      </ColorGauge>
    </div>
  </div>
);
