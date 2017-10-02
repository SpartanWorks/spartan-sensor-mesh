function failPromise(p: Promise<Response>): Promise<Response> {
  return new Promise<Response>((resolve, reject) => {
    p.then((resp) => (resp.ok) ? resolve(resp) : reject(resp)).catch(reject);
  });
}

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

    return failPromise(fetch(this.baseUrl + "/api/login", {
      method: "GET",
      headers: this.makeHeaders()
    }));
  }

  setupWifi(ssid: string, password: string) {
    return failPromise(fetch("/api/config?ssid=" + encodeURI(ssid) + "&pass=" + encodeURI(password), {
      method: "GET",
      headers: this.makeHeaders()
    }));
  }
}
