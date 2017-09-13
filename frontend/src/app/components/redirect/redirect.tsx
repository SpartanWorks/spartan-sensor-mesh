import * as preact from "preact";
import { actions } from "../../actions/flow";
import * as styles from "./redirect.css";

interface Props {
  to: string;
}

export class RedirectButton extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.mainButton} onClick={() => actions.redirectionRequested(this.props.to)}/>
    );
  }
}
