import * as preact from "preact";
import { route } from "preact-router";
import * as styles from "./redirect.css";

interface Props {
  to: string;
}

export class RedirectButton extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.wrapper}>
        <div className={styles.button} onClick={() => route(this.props.to)}/>
      </div>
    );
  }
}
