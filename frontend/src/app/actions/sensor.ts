import { action } from "mobx";
import { SensorData } from "../services/sensor";
import { MainStore, mainStore } from "../store/main";

export class SensorActions {
  store: MainStore;

  constructor(store: MainStore) {
    this.store = store;
  }

  @action.bound
  dataRetrieved(data: SensorData) {
    this.store.data = data;
  }

}

export const actions = new SensorActions(mainStore);
