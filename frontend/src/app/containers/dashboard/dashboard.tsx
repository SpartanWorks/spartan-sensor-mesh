import { observer } from "mobx-observer";
import * as preact from "preact";
import { ClimateWidget } from "../../components/climate/climate";
import { iconCogs, RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { MainStore } from "../../store/main";
import * as styles from "./dashboard.css";

interface Props {
  store: MainStore;
}

@observer
export class Dashboard extends preact.Component<Props, {}> {
  render() {
    return (
      <div className={styles.mainWrapper}>
        {
          (!this.props.store.dataLoaded) ? (
            <Spinner/>
          ) : (
            <ClimateWidget data={this.props.store.data.sensors[0]} // FIXME Render all sensors according to their type.
                           minTemperature={11}
                           maxTemperature={37}/>
          )
        }
        <RedirectButton to={"/config"} icon={iconCogs} tooltip="Change configuration parameters."/>
      </div>
    );
  }
}
