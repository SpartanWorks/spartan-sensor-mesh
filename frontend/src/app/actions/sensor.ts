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
    this.store.humidity = data.humidity;
    this.store.temperature = data.temperature;
  }

}

export const actions = new SensorActions(mainStore);
