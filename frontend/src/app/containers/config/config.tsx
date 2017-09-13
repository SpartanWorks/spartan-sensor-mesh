import * as preact from "preact";
import * as styles from "./config.css";

export class Config extends preact.Component<{}, any> {
  onSubmit(event: Event) {
    let ssid = (document.querySelector("#config-ssid") as any).value;
    let pass = (document.querySelector("#config-pass") as any).value;
    fetch("/api/config?ssid=" + encodeURI(ssid) + "&pass=" + encodeURI(pass))
      .then((response) => response.json())
      .then(() => {
        alert("Saved!");
      }).catch(() => {
        alert("Could not save the network configuration!");
      });
    event.preventDefault();
    return false;
  }

  render() {
    return (
      <div className={styles.mainWrapper}>
        <form id="config-form" onSubmit={this.onSubmit}>
          <fieldset>
            <legend>Network configuration:</legend>
            <div>
              <input id="config-ssid" type="text" name="ssid" placeholder="Network SSID" pattern=".+" required/>
            </div>
            <div>
              <input id="config-pass" type="password" name="pass" placeholder="p4$$w0rd" pattern=".+" required/>
            </div>
            <div>
              <input type="submit" value="Save"/>
            </div>
          </fieldset>
        </form>
      </div>
    );
  }
}
