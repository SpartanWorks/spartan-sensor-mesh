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
    this.onUpdateCallback = () => {
      // Do nothing.
    };
    this.baseUrl = baseUrl;
   }

  connect(interval: number) {
    this.fetchData().then(() => {
      setInterval(this.fetchData.bind(this), interval);
    });
  }

  private fetchData(): Promise<void> {
    return fetch(this.baseUrl + "/api/sensor")
      .then((r) => r.json())
      .then(this.onUpdateCallback)
      .catch((e) => {
        console.error({
          error: "Fetching data failed.",
          cause: e
        });
      });
  }

  onUpdate(callback: UpdateCallback) {
    this.onUpdateCallback = callback;
  }
}
