import { action, observable } from "mobx";
import { ConfigService } from "../services/config";

export type LoginState = "logged-out" | "in-progress" | "failed" | "success";
export type WifiSetupState = "assumed-ok" | "in-progress" | "failed" | "success";

export class ConfigStore {
  private config: ConfigService;

  @observable
  loginState: LoginState = "logged-out";

  @observable
  wifiSetupState: WifiSetupState = "assumed-ok";

  constructor(config: ConfigService) {
    this.config = config;
  }

  @action.bound
  logIn(login: string, password: string) {
    this.loginState = "in-progress";

    this.config.logIn(login, password).then((resp) => {
      this.loginSuccessful();
    }).catch((e) => {
      console.error({
        error: "Login failed.",
        cause: e
      });
      this.loginFailed();
    });
  }

  @action.bound
  loginSuccessful() {
    this.loginState = "success";
  }

  @action.bound
  loginFailed() {
    this.loginState = "failed";
  }

  @action.bound
  setupWifi(ssid: string, pass: string) {
    this.wifiSetupState = "in-progress";

    this.config.setupWifi(ssid, pass).then((resp) => {
      this.wifiSetupSuccessful();
    }).catch((e) => {
      this.wifiSetupFailed();
      console.error({
        error: "Could not setup WiFi.",
        cause: e
      });
    });
  }

  @action.bound
  wifiSetupSuccessful() {
    this.wifiSetupState = "success";
  }

  @action.bound
  wifiSetupFailed() {
    this.wifiSetupState = "failed";
  }

}
