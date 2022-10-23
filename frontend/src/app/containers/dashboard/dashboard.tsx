import { observer } from "mobx-preact";
import * as preact from "preact";
import { CO2 } from "../../components/co2/co2";
import { Humidity } from "../../components/humidity/humidity";
import { PM } from "../../components/pm/pm";
import { Pressure } from "../../components/pressure/pressure";
import { iconCogs, RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { Temperature } from "../../components/temperature/temperature";
import { UnsupportedSensor } from "../../components/unsupported/unsupported";
import { SensorData } from "../../services/device";
import { DashboardStore } from "../../store/dashboard";
import * as styles from "./dashboard.css";

interface Props {
  store: DashboardStore;
}

function renderSensor(data: SensorData) {
  switch (data.type) {
  case "temperature":
    return <Temperature data={data} min={11} max={37}/>;
  case "humidity":
    return <Humidity data={data}/>;
  case "pressure":
    return <Pressure data={data} min={95000} max={105000}/>;
  case "pm2.5":
    return <PM data={data} min={0} max={250}/>;
  case "pm10":
    return <PM data={data} min={0} max={500}/>;
  case "co2":
    return <CO2 data={data} min={0} max={2000}/>;
  default:
    return <UnsupportedSensor data={data}/>;
  }
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
