import * as preact from "preact";
import { Main } from "./containers/main/main";
import * as styles from "./main.css";
import { SensorService } from "./services/sensor";
import { MainStore } from "./store/main";

export function onLoad() {
  const mainStore = new MainStore();

  const sensorService = new SensorService(window.location.protocol + "//" + window.location.host);
  sensorService.onUpdate(mainStore.dataRetrieved);

  const container = document.createElement("div");
  container.classList.add(styles.appContainer);
  document.body.appendChild(container);

  preact.render(<Main store={mainStore}/>, container);
  console.log("App successfully loaded!");
}

(function () {
  if (document.readyState === "interactive") {
    onLoad();
  } else {
    document.addEventListener("DOMContentLoaded", onLoad);
  }
})();
