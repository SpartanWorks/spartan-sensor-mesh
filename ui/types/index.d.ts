declare module "mobx-preact" {
  import { ComponentConstructor } from "preact";
  type Component<P = any> = ComponentConstructor<P>;
  export declare function observer<C extends Component>(constructor: C): C;
}
