export interface Metric {
  avg: number;
  max: number;
  min: number;
  val: number;
  var: number;
}

export interface SensorData {
  humidity: Metric;
  temperature: Metric;
  errors: number;
  measurements: number;
  status: "ok" | "error";
}

export interface UpdateCallback {
  (arg: SensorData): void;
}

export class SensorService {
  baseUrl: string;
  onUpdateCallback: UpdateCallback;

  constructor(baseUrl: string) {
    this.baseUrl = baseUrl;
    this.connect(2000);
  }

  private connect(interval: number) {
    setInterval(() => {
      if (this.onUpdateCallback) {
        fetch(this.baseUrl + "/api/sensor")
          .then((r) => r.json())
          .then(this.onUpdateCallback)
          .catch((e) => {
            console.error({
              error: "Fetching data failed.",
              cause: e
            });
          });
      }
    }, interval);
  }

  onUpdate(callback: UpdateCallback) {
    this.onUpdateCallback = callback;
  }
}
