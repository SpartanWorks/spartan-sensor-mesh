import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { ColorGauge } from "../gauge/color";
import { Label } from "../gauge/label";

interface Props {
  data: SensorData;
  min: number;
  max: number;
}

export const VOC = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <ColorGauge value={props.data.reading.mean}
                  min={props.min}
                  max={props.max}
                  variance={props.data.reading.variance}
                  color="#BADD1E"
                  isError={props.data.status === "error"}
                  errorTooltip="Sensor is not responding.">
        <Label name={props.data.name[0].toUpperCase() + props.data.name.substring(1)}
               value={Math.round(props.data.reading.mean)}
               unit="ppb"
               tooltip={"Averaged from last " + props.data.reading.samples + " readings."}/>
      </ColorGauge>
    </div>
  </div>
);
