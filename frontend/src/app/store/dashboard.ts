import { action, observable } from "mobx";
import { DeviceData } from "../services/device";

export class DashboardStore {
  @observable
  dataLoaded = false;

  @observable
  data: DeviceData = {
    model: "UNKNOWN",
    name: "UNKNOWN",
    group: "UNKNOWN",
    sensors: []
  };

  @action.bound
  dataRetrieved(data: DeviceData) {
    this.dataLoaded = true;
    this.data = data;
  }
}
