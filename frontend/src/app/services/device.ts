export interface SensorValueStats {
  mean: number;
  variance: number;
  samples: number;
  maximum: number;
  minimum: number;
}

export interface SensorValueRange {
  minimum: number;
  maximum: number;
}

export interface SensorValue {
  unit: string;
  last: number;
  range: SensorValueRange;
  stats: SensorValueStats;
}

export interface SensorReading {
  type: string;
  name: string;
  model: string;
  status: string;
  errors: number;
  lastError: string;
  measurements: number;
  value: SensorValue;
}

export interface DeviceData {
  model: string;
  name: string;
  group: string;
  sensors: Array<SensorReading>;
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
    this.fetchInterval(interval);
  }

  fetchData(): Promise<DeviceData> {
    return fetch(this.baseUrl + "/api/data").then((r) => r.json());
  }

  private fetchInterval(interval: number): Promise<void> {
    const repeat = () => {
      setTimeout(() => this.fetchInterval(interval), interval);
    };

    return this.fetchData()
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
