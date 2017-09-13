import * as preact from "preact";
import { route, Router } from "preact-router";
import { ConfigContainer } from "./containers/config/configContainer";
import { Dashboard } from "./containers/dashboard/dashboard";
import * as styles from "./main.css";
import { mainStore } from "./store/main";

interface RouteProps {
  path?: string;
  default?: boolean;
}

const DashboardRoute = (p: RouteProps) => (
  <Dashboard store={mainStore}/>
);

const ConfigRoute = (p: RouteProps) => (
  <ConfigContainer/>
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
  console.log("App successfully loaded!");
  const container = document.createElement("div");
  container.classList.add(styles.appContainer);
  document.body.appendChild(container);
  preact.render(<Main/>, container);
}

(function () {
  const DOMContentLoaded = document.readyState === "interactive";

  if (DOMContentLoaded) {
    onLoad();
  } else {
    document.addEventListener("DOMContentLoaded", onLoad);
  }
})();