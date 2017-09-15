import * as preact from "preact";
import { route } from "preact-router";
import * as styles from "./redirect.css";

interface Props {
  to: string;
}

export const RedirectButton = (props: Props) => (
  <div className={styles.wrapper}>
    <div className={styles.button} onClick={() => route(props.to)}/>
  </div>
);
