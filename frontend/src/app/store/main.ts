import { observable } from "mobx";

export class MainStore {
  @observable
  duckIsVisible = false;
}

export const mainStore = new MainStore();
