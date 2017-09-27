import { observer } from "mobx-observer";
import * as preact from "preact";
import { ClimateWidget } from "../../components/climate/climate";
import { iconCogs, RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { UnsupportedSensor } from "../../components/unsupported/unsupported";
import { SensorData } from "../../services/device";
import { MainStore } from "../../store/main";
import * as styles from "./dashboard.css";

interface Props {
  store: MainStore;
}

function selectSensor(data: SensorData) {
  switch (data.type) {
  case "DHT":
    return <ClimateWidget data={data} minTemperature={11} maxTemperature={37}/>;
  default:
    return <UnsupportedSensor data={data}/>;
  }
}

function renderSensor(data: SensorData) {
  return (
    <div className={styles.widgetWrapper}>
      { selectSensor(data) }
    </div>
  );
}

@observer
export class Dashboard extends preact.Component<Props, {}> {
  render() {
    return (
      <div className={styles.mainWrapper}>
        <div className={styles.displayWrapper}>
          { !this.props.store.dataLoaded ? <Spinner/> : this.props.store.data.sensors.map(renderSensor) }
        </div>
        <RedirectButton to={"/config"} icon={iconCogs} tooltip="Change configuration parameters."/>
      </div>
    );
  }
}
