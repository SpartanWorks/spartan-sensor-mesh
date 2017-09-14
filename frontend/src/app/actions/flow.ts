import { action } from "mobx";
import { route } from "preact-router";
import { MainStore, mainStore } from "../store/main";

class FlowActions {
  store: MainStore;

  constructor(store: MainStore) {
    this.store = store;
  }

  @action.bound
  redirectionRequested(to: string) {
    route(to);
  }

}

export const actions = new FlowActions(mainStore);
