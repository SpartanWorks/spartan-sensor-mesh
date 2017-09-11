import { observer } from "mobx-observer";
import * as preact from "preact";
import * as styles from "./duckContainer.css";

interface Props {
  duckIsVisible: boolean;
}

@observer
export class DuckContainer extends preact.Component<Props, any> {
  props: Props;

  constructor(props: Props) {
    super(props);
    this.props = props;
  }

  render () {
    const isVisible = this.props.duckIsVisible ? styles.visible : styles.hidden;
    return (
        <div className={styles.duckWrapper + " " + " " + isVisible}>
          <div className={styles.duck}/>
        </div>
    );
  }
}
