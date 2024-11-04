import { action, observable } from "mobx";
import { DeviceData } from "../services/device";

export class DeviceDataWithState {
  data: DeviceData;
  stale: boolean;
}

export class DashboardStore {
  @observable
  dataLoaded = false;

  @observable
  data: DeviceDataWithState[] = [];

  @action.bound
  dataRetrieved(data: DeviceData) {
    this.dataLoaded = true;
    this.data = [{data, stale: false}];
  }

  @action.bound
  meshDataRetrieved(data: DeviceData[]) {
    this.dataLoaded = true;
    const oldData = this.data;
    this.data = data.map((d) => ({ data: d, stale: false }));
    oldData.forEach((old) => {
      if (!data.find((d) => d.model === old.data.model && d.name === old.data.name && d.group === old.data.group)) {
        this.data.push({data: old.data, stale: true});
      }
    });
  }
}
