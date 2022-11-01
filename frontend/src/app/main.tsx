import * as preact from "preact";
import { Main } from "./containers/main/main";
import * as styles from "./main.css";
import { ConfigService } from "./services/config";
import { MeshService } from "./services/mesh";
import { ConfigStore } from "./store/config";
import { DashboardStore } from "./store/dashboard";

export function onLoad() {
  const baseUrl = window.location.protocol + "//" + window.location.host;
  const configService = new ConfigService(baseUrl);
  const meshService = new MeshService(baseUrl);

  const dashboardStore = new DashboardStore();
  const configStore = new ConfigStore(configService);

  meshService.onUpdate(dashboardStore.meshDataRetrieved);
  meshService.connect(60000, 5000);

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
