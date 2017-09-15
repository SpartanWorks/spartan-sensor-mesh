import { action, observable } from "mobx";
import { SensorData } from "../services/sensor";

export class MainStore {
  @observable
  data: SensorData = {
    status: "ok",
    measurements: 0,
    errors: 0,
    humidity: {
      avg: 0,
      max: 0,
      min: 0,
      val: 0,
      var: 0
    },
    temperature: {
      avg: 0,
      max: 0,
      min: 0,
      val: 0,
      var: 0
    },
  };

  @action.bound
  dataRetrieved(data: SensorData) {
    this.data = data;
  }
}
