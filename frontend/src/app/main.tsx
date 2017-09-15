import * as preact from "preact";
import { route, Router } from "preact-router";
import { Config } from "./containers/config/config";
import { Dashboard } from "./containers/dashboard/dashboard";
import * as styles from "./main.css";
import { SensorService } from "./services/sensor";
import { MainStore } from "./store/main";

interface RouteProps {
  path?: string;
  default?: boolean;
}

interface RouteWithStore extends RouteProps {
  store: MainStore;
}

const DashboardRoute = (p: RouteWithStore) => (
  <Dashboard store={p.store}/>
);

const ConfigRoute = (p: RouteProps) => (
  <Config/>
);

class NotFoundRoute extends preact.Component<RouteProps, {}> {
  componentWillMount() {
    route("/", true);
  }

  render() {
    return (
      <div />
    );
  }
}

const Main = (p: RouteWithStore) => (
  <Router>
    <DashboardRoute store={p.store} path="/"/>
    <ConfigRoute path="/config"/>
    <NotFoundRoute default/>
  </Router>
);

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
