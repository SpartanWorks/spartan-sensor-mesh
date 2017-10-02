export class ConfigService {
  baseUrl: string;
  credentials: string;

  constructor(baseUrl: string) {
    this.baseUrl = baseUrl;
   }

  logIn(login: string, password: string) {
    this.credentials = btoa(login + ":" + password);

    return fetch(this.baseUrl + "/api/login", {
      method: "GET",
      headers: {
        "Authorization": "Basic " + this.credentials
      }
    });
  }
}
