import * as preact from "preact";
import { Main } from "./containers/main/main";
import * as styles from "./main.css";
import { DeviceService } from "./services/device";
import { DashboardStore } from "./store/dashboard";

export function onLoad() {
  const dashboardStore = new DashboardStore();
  const deviceService = new DeviceService(window.location.protocol + "//" + window.location.host);

  deviceService.onUpdate(dashboardStore.dataRetrieved);
  deviceService.connect(5000);

  const container = document.createElement("div");
  container.classList.add(styles.appContainer);
  document.body.appendChild(container);

  preact.render(<Main dashboardStore={dashboardStore}/>, container);
  console.log("App successfully loaded!");
}

(function () {
  if (document.readyState === "interactive") {
    onLoad();
  } else {
    document.addEventListener("DOMContentLoaded", onLoad);
  }
})();
