import * as preact from "preact";
import { SensorData } from "../../services/device";
import * as styles from "../../styles/widget.css";
import { SensorLabel } from "../gauge/label";
import { Tier, TieredGauge } from "../gauge/tiered";

export const PM_10 = "pm10";
export const PM_25 = "pm2.5";

const PM_10_TIERS: Tier[] = [
  { min: 0, max: 54, color: "#00e400" },
  { min: 54, max: 154, color: "#ff0" },
  { min: 154, max: 254, color: "#ff7e00" },
  { min: 254, max: 354, color: "#f00" },
  { min: 354, max: 424, color: "#8f3f97" },
];

const PM_25_TIERS: Tier[] = [
  { min: 0, max: 12, color: "#00e400" },
  { min: 12, max: 35, color: "#ff0" },
  { min: 35, max: 55, color: "#ff7e00" },
  { min: 55, max: 150, color: "#f00" },
  { min: 150, max: 199, color: "#8f3f97" },
];

const PM_DEFAULT_COLOR = "#7e0023";

interface Props {
  data: SensorData;
  min: number;
  max: number;
}

export const PM = (props: Props) => (
  <div className={styles.widgetWrapper}>
    <div className={styles.readingWrapper}>
      <TieredGauge value={props.data.reading.mean}
                   variance={props.data.reading.variance}
                   tiers={props.data.type === PM_10 ? PM_10_TIERS : PM_25_TIERS}
                   defaultTier={{ min: props.min, max: props.max, color: PM_DEFAULT_COLOR }}
                   isError={props.data.status === "error"}
                   errorTooltip={props.data.lastError}>
        <SensorLabel data={props.data}/>
      </TieredGauge>
    </div>
  </div>
);
