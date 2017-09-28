import * as preact from "preact";
import { Main } from "./containers/main/main";
import * as styles from "./main.css";
import { DeviceService } from "./services/device";
import { MainStore } from "./store/main";

export function onLoad() {
  const mainStore = new MainStore();

  const deviceService = new DeviceService(window.location.protocol + "//" + window.location.host);
  deviceService.onUpdate(mainStore.dataRetrieved);
  deviceService.connect(5000);

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
