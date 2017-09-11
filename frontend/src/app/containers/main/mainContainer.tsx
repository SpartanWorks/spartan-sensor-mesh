import { observer } from "mobx-observer";
import * as preact from "preact";
import { userActions } from "../../actions/user";
import { MainStore } from "../../store/main";
import { DuckContainer } from "../duck/duckContainer";
import * as styles from "./mainContainer.css";

interface Props {
  store: MainStore;
}

@observer
export class MainContainer extends preact.Component<Props, any> {
  props: Props;

  constructor(props: Props) {
    super(props);
    this.props = props;
  }

  render () {
    return (
      <div className={styles.mainWrapper}>
        <div className={styles.mainButton} onClick={userActions.mainButtonClicked}/>
        <DuckContainer duckIsVisible={this.props.store.duckIsVisible}/>
      </div>
    );
  }
}
