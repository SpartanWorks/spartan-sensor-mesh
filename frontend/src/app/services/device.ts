export interface SensorReading {
  value: number;
  mean: number;
  variance: number;
  samples: number;
  maximum?: number;
  minimum?: number;
}

export interface SensorData {
  type: string;
  status: "ok" | "error";
  errors: number;
  measurements: number;
  readings: {
    [name: string]: SensorReading;
  };
}

export interface DeviceData {
  model: string;
  name: string;
  group: string;
  sensors: Array<SensorData>;
}

export interface UpdateCallback {
  (arg: DeviceData): void;
}

export class DeviceService {
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
    const repeat = () => {
      setTimeout(() => this.fetchData(interval), interval);
    };

    return fetch(this.baseUrl + "/api/data")
      .then((r) => r.json())
      .then(this.onUpdateCallback)
      .then(repeat)
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
