import * as preact from "preact";
import { route, Router } from "preact-router";
import { actions } from "./actions/sensor";
import { Config } from "./containers/config/config";
import { Dashboard } from "./containers/dashboard/dashboard";
import * as styles from "./main.css";
import { SensorService } from "./services/sensor";
import { mainStore } from "./store/main";

interface RouteProps {
  path?: string;
  default?: boolean;
}

const DashboardRoute = (p: RouteProps) => (
  <Dashboard store={mainStore}/>
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

const Main = () => (
    <Router>
      <DashboardRoute path="/"/>
      <ConfigRoute path="/config"/>
      <NotFoundRoute default/>
    </Router>
);

export function onLoad() {
  const sensorService = new SensorService();
  sensorService.onUpdate((d) => actions.dataRetrieved(d));
  const container = document.createElement("div");
  container.classList.add(styles.appContainer);
  document.body.appendChild(container);
  preact.render(<Main/>, container);
  console.log("App successfully loaded!");
}

(function () {
  if (document.readyState === "interactive") {
    onLoad();
  } else {
    document.addEventListener("DOMContentLoaded", onLoad);
  }
})();
