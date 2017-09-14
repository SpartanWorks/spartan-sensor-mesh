import { observable } from "mobx";

export class MainStore {
  @observable
  humidity = 0;

  @observable
  temperature = 0;
}

export const mainStore = new MainStore();
