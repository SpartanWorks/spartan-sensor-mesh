import { action, observable } from "mobx";
import { DeviceData } from "../services/device";

export class DashboardStore {
  @observable
  dataLoaded = false;

  @observable
  data: DeviceData[] = [];

  @action.bound
  dataRetrieved(data: DeviceData) {
    this.dataLoaded = true;
    this.data = [data];
  }

  @action.bound
  meshDataRetrieved(data: DeviceData[]) {
    this.dataLoaded = true;
    this.data = data;
  }
}
