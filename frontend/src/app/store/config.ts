import { action, observable } from "mobx";
import { ConfigService } from "../services/config";

export type LoginState = "logged-out" | "in-progress" | "failed" | "success";

export class ConfigStore {
  private config: ConfigService;

  @observable
  loginState: LoginState = "logged-out";

  constructor(config: ConfigService) {
    this.config = config;
  }

  @action.bound
  logIn(login: string, password: string) {
    this.loginState = "in-progress";

    this.config.logIn(login, password).then((resp) => {
      if (resp.ok) {
        this.loginSuccessful();
      } else {
        console.error({
          error: "Login failed.",
          response: resp
        });
        this.loginFailed();
      }
    }).catch((e) => {
      console.error({
        error: "Login failed.",
        response: e
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
}
