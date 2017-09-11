import * as preact from "preact";
import { MainContainer } from "./containers/main/mainContainer";
import { mainStore } from "./store/main";

export let hurr = "23";
export function onLoad() {
  console.log("App successfully loaded!");
  const container = document.createElement("div");
  document.body.appendChild(container);
  preact.render(<MainContainer store={mainStore}/>, container);
}

(function () {
  const DOMContentLoaded = document.readyState === "interactive";

  if (DOMContentLoaded) {
    onLoad();
  } else {
    document.addEventListener("DOMContentLoaded", onLoad);
  }
})();
