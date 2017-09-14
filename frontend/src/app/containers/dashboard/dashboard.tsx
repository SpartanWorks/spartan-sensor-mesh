import { observer } from "mobx-observer";
import * as preact from "preact";
import { ClimateWidget } from "../../components/climate/climate";
import { RedirectButton } from "../../components/redirect/redirect";
import { MainStore } from "../../store/main";
import * as styles from "./dashboard.css";

interface Props {
  store: MainStore;
}

@observer
export class Dashboard extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.mainWrapper}>
        <ClimateWidget humidity={this.props.store.humidity} temperature={this.props.store.temperature}/>
        <RedirectButton to={"/config"}/>
      </div>
    );
  }
}
