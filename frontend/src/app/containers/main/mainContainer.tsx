import { observer } from "mobx-observer";
import * as preact from "preact";
import { RedirectButton } from "../../components/redirect/redirect";
import { MainStore } from "../../store/main";
import * as styles from "./mainContainer.css";

interface Props {
  store: MainStore;
}

@observer
export class MainContainer extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.mainWrapper}>
        <RedirectButton to={"/config"}/>
      </div>
    );
  }
}
