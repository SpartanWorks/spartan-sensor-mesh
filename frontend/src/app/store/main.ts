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
      }
    ]
  };

  @action.bound
  dataRetrieved(data: DeviceData) {
    this.dataLoaded = true;
    this.data = data;
  }
}
