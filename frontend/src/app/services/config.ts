export class ConfigService {
  baseUrl: string;
  credentials: string;

  constructor(baseUrl: string) {
    this.baseUrl = baseUrl;
   }

  private makeHeaders() {
    return {
      "Authorization": "Basic " + this.credentials
    };
  }

  logIn(login: string, password: string) {
    this.credentials = btoa(login + ":" + password);

    return fetch(this.baseUrl + "/api/login", {
      method: "GET",
      headers: this.makeHeaders()
    });
  }

  setupWifi(ssid: string, password: string) {
    return fetch("/api/config?ssid=" + encodeURI(ssid) + "&pass=" + encodeURI(password), {
      method: "GET",
      headers: this.makeHeaders()
    });
  }
}
