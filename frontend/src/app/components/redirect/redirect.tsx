import * as preact from "preact";
import { actions } from "../../actions/flow";
import * as styles from "./redirect.css";

interface Props {
  to: string;
}

export class RedirectButton extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.wrapper}>
        <div className={styles.button} onClick={() => actions.redirectionRequested(this.props.to)}/>
      </div>
    );
  }
}
