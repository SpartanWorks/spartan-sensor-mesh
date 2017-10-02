import * as preact from "preact";
import { Main } from "./containers/main/main";
import * as styles from "./main.css";
import { ConfigService } from "./services/config";
import { DeviceService } from "./services/device";
import { ConfigStore } from "./store/config";
import { DashboardStore } from "./store/dashboard";

export function onLoad() {
  const baseUrl = window.location.protocol + "//" + window.location.host;
  const configService = new ConfigService(baseUrl);
  const deviceService = new DeviceService(baseUrl);

  const dashboardStore = new DashboardStore();
  const configStore = new ConfigStore(configService);

  deviceService.onUpdate(dashboardStore.dataRetrieved);
  deviceService.connect(5000);

  const container = document.createElement("div");
  container.classList.add(styles.appContainer);
  document.body.appendChild(container);

  preact.render(<Main dashboardStore={dashboardStore}
                      configStore={configStore}/>,
                container);
  console.log("App successfully loaded!");
}

(function () {
  if (document.readyState === "interactive") {
    onLoad();
  } else {
    document.addEventListener("DOMContentLoaded", onLoad);
  }
})();
