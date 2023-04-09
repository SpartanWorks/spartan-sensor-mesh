import * as preact from "preact";
import { route, Router } from "preact-router";
import { ConfigStore } from "../../store/config";
import { DashboardStore } from "../../store/dashboard";
import { Config } from "../config/config";
import { Dashboard } from "../dashboard/dashboard";

interface RouteProps {
  path?: string;
  default?: boolean;
}

interface RouteWithStore<S> extends RouteProps {
  store: S;
}

const DashboardRoute = (p: RouteWithStore<DashboardStore>) => (
  <Dashboard store={p.store}/>
);

const ConfigRoute = (p: RouteWithStore<ConfigStore>) => (
  <Config store={p.store}/>
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

interface Props {
  dashboardStore: DashboardStore;
  configStore: ConfigStore;
}

export const Main = (p: Props) => (
  <Router>
    <DashboardRoute store={p.dashboardStore} path="/"/>
    <ConfigRoute store={p.configStore} path="/config"/>
    <NotFoundRoute default/>
  </Router>
);
