import { action, observable } from "mobx";
import { DeviceData } from "../services/device";

export class MainStore {
  @observable
  dataLoaded = false;

  @observable
  data: DeviceData = {
    model: "UNKNOWN",
    name: "UNKNOWN",
    group: "UNKNOWN",
    sensors: [
      {
        type: "UNKNOWN",
        status: "error",
        errors: 0,
        measurements: 0,
        readings: {},
      }
    ]
  };

  @action.bound
  dataRetrieved(data: DeviceData) {
    this.dataLoaded = true;
    this.data = data;
  }
}
