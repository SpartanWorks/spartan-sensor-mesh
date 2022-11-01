import { DeviceData, DeviceService } from "./device";

export interface MeshNode {
  hostname: string;
  ip: string;
  port: number;
}

export interface MeshUpdateCallback {
  (arg: DeviceData[]): void;
}

export class MeshService {
  baseUrl: string;
  nodes: MeshNode[];
  devices: DeviceService[];
  onUpdateCallback: MeshUpdateCallback;

  constructor(baseUrl: string) {
    this.onUpdateCallback = () => {
      // Do nothing.
    };
    this.baseUrl = baseUrl;
    this.nodes = [];
    this.devices = [];
   }

  connect(discoveryInterval: number, fetchInterval: number) {
    this.discoverNodes(discoveryInterval).then(() => this.fetchAll(fetchInterval));
  }

  private discoverNodes(interval: number): Promise<void> {
    const repeat = () => {
      setTimeout(() => this.discoverNodes(interval), interval);
    };

    return fetch(this.baseUrl + "/api/mesh")
      .then((r) => r.json())
      .then((nodes: MeshNode[]) => {
        this.nodes = nodes;
        this.devices = nodes.map((n) => new DeviceService("http://" + n.ip + ":" + n.port));
      })
      .then(repeat)
      .catch((e) => {
        console.error({
          error: "Mesh node discovery failed.",
          cause: e
        });
        repeat();
      });
  }

  private fetchAll(interval: number) {
    const repeat = () => {
      setTimeout(() => this.fetchAll(interval), interval);
    };

    return Promise.all(this.devices.map((d) => d.fetchData()))
      .then(this.onUpdateCallback)
      .then(repeat)
      .catch((e) => {
        console.error({
          error: "Fetching mesh data failed.",
          cause: e
        });
        repeat();
      });
  }

  onUpdate(callback: MeshUpdateCallback) {
    this.onUpdateCallback = callback;
  }
}
