import { observer } from "mobx-observer";
import * as preact from "preact";
import { ClimateWidget } from "../../components/climate/climate";
import { RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { MainStore } from "../../store/main";
import * as styles from "./dashboard.css";

interface Props {
  store: MainStore;
}

@observer
export class Dashboard extends preact.Component<Props, {}> {
  render() {
    if (!this.props.store.dataLoaded) {
      return (
        <div className={styles.mainWrapper}>
          <Spinner/>
        </div>
      );
    } else {
      return (
        <div className={styles.mainWrapper}>
          <ClimateWidget humidity={this.props.store.data.humidity.val}
                         temperature={this.props.store.data.temperature.val}
                         minTemperature={11}
                         maxTemperature={37}/>
          <RedirectButton to={"/config"}/>
        </div>
      );
    }
  }
}
