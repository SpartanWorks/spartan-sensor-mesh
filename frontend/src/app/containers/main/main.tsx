import * as preact from "preact";
import { route, Router } from "preact-router";
import { MainStore } from "../../store/main";
import { Config } from "../config/config";
import { Dashboard } from "../dashboard/dashboard";

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

export const Main = (p: RouteWithStore) => (
  <Router>
    <DashboardRoute store={p.store} path="/"/>
    <ConfigRoute path="/config"/>
    <NotFoundRoute default/>
  </Router>
);
