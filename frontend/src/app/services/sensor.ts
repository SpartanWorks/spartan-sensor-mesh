export interface SensorData {
  humidity: number;
  temperature: number;
}

export interface UpdateCallback {
  (arg: SensorData): void;
}

export class SensorService {
  onUpdateCallback: UpdateCallback;

  constructor() {
    setInterval(() => {
      if (this.onUpdateCallback) {
        this.onUpdateCallback({
          humidity: Math.floor(Math.random() * 100 + 1),
          temperature: Math.floor(Math.random() * 35 + 1)
        });
      }
    }, 2000);
  }

  onUpdate(callback: UpdateCallback) {
    this.onUpdateCallback = callback;
  }
}
