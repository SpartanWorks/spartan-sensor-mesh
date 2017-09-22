import { action, observable } from "mobx";
import { SensorData } from "../services/sensor";

export class MainStore {
  @observable
  dataLoaded = false;

  @observable
  data: SensorData = {
    type: "UNKNOWN",
    status: "ok",
    measurements: 0,
    errors: 0,
    humidity: {
      value: 0,
      variance: 0,
      mean: 0,
      samples: 0
    },
    temperature: {
      value: 0,
      variance: 0,
      mean: 0,
      samples: 0
    },
  };

  @action.bound
  dataRetrieved(data: SensorData) {
    this.dataLoaded = true;
    this.data = data;
  }
}
