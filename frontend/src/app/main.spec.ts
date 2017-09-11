import * as preact from "preact";
import * as main from "./main";

describe("main", () => {
  it("should render stuff", () => {
    spyOn(preact, "render");
    main.onLoad();
    expect(preact.render).toHaveBeenCalled();
  });
});
