import { observer } from "mobx-observer";
import * as preact from "preact";
import { iconChart, RedirectButton } from "../../components/redirect/redirect";
import { Spinner } from "../../components/spinner/spinner";
import { ConfigStore } from "../../store/config";
import * as styles from "./config.css";

interface ConfigFormProps {
  onSubmit(ssid: string, pass: string): void;
}

const ConfigForm = (props: ConfigFormProps) => {
  const onSubmit = (event: Event) => {
    let ssid = (document.querySelector("#config-ssid") as any).value;
    let pass = (document.querySelector("#config-pass") as any).value;
    props.onSubmit(ssid, pass);
    event.preventDefault();
    return false;
  };
  return (
    <form onSubmit={onSubmit}>
      <fieldset>
        <legend>Network configuration:</legend>
        <input id="config-ssid" type="text" name="ssid" placeholder="Network SSID" pattern=".+" required/>
        <input id="config-pass" type="password" name="pass" placeholder="p4$$w0rd" pattern=".+" required/>
        <input type="submit" value="Save"/>
      </fieldset>
    </form>
  );
};

interface LoginFormProps {
  onSubmit(login: string, pass: string): void;
}

const LoginForm = (props: LoginFormProps) => {
    const onSubmit = (event: Event) => {
    let ssid = (document.querySelector("#config-login") as any).value;
    let pass = (document.querySelector("#config-pass") as any).value;
    props.onSubmit(ssid, pass);
    event.preventDefault();
    return false;
  };
  return (
    <form onSubmit={onSubmit}>
      <fieldset>
        <legend>Log in:</legend>
        <input id="config-login" type="text" name="login" placeholder="Login" pattern=".+" required/>
        <input id="config-pass" type="password" name="pass" placeholder="p4$$w0rd" pattern=".+" required/>
        <input type="submit" value="Login"/>
      </fieldset>
    </form>
  );
};

function renderLoginState(store: ConfigStore) {
  switch (store.loginState) {
  case "logged-out":
  case "failed":
    return <LoginForm onSubmit={store.logIn}/>;
  case "success":
    return <ConfigForm onSubmit={(ssid: string, pass: string) => {
      fetch("/api/config?ssid=" + encodeURI(ssid) + "&pass=" + encodeURI(pass))
        .then((response) => response.json())
        .then(() => {
          alert("Saved!");
        }).catch(() => {
          alert("Could not save the network configuration!");
        });
    }}/>;
  default:
    return <Spinner/>;
  }
}

interface Props {
  store: ConfigStore;
}

@observer
export class Config extends preact.Component<Props, any> {
  render() {
    return (
      <div className={styles.mainWrapper}>
        <div className={styles.displayWrapper}>
        { renderLoginState(this.props.store) }
        </div>
        <RedirectButton to={"/"} icon={iconChart} tooltip="Go back to sensor readings."/>
      </div>
    );
  }
}
