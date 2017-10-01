export class ConfigService {
  baseUrl: string;
  headers: Headers;

  constructor(baseUrl: string) {
    this.baseUrl = baseUrl;
    this.headers = new Headers();
   }

  logIn(login: string, password: string) {
    this.headers.append("Authorization", "Basic " + btoa(login + ":" + password));

    return fetch(this.baseUrl + "/api/login", {
      method: "GET",
      headers: this.headers
    });
  }
}
