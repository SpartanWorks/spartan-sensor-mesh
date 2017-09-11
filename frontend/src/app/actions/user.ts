import { action } from "mobx";
import { MainStore, mainStore } from "../store/main";

class UserActions {
  store: MainStore;

  constructor(store: MainStore) {
    this.store = store;
  }

  @action.bound
  mainButtonClicked() {
    this.store.duckIsVisible = !this.store.duckIsVisible;
  }

}

export const userActions = new UserActions(mainStore);
