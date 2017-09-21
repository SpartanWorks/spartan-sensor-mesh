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
    this.fetchData(interval);
  }

  private fetchData(interval: number): Promise<void> {
    const repeat = () => this.fetchData(interval);

    return fetch(this.baseUrl + "/api/sensor")
      .then((r) => r.json())
      .then(this.onUpdateCallback)
      .then(() => {
        setTimeout(repeat, interval);
      })
      .catch((e) => {
        console.error({
          error: "Fetching data failed.",
          cause: e
        });
        repeat();
      });
  }

  onUpdate(callback: UpdateCallback) {
    this.onUpdateCallback = callback;
  }
}
