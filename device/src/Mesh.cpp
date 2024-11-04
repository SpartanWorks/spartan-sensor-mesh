#include "Mesh.hpp"

Mesh::Mesh(): hosts(nullptr) {}

Mesh::~Mesh() {
  if (this->hosts != nullptr) {
    delete this->hosts;
  }
}

void Mesh::addHost(Host h) {
  this->hosts = new List<Host>(h, this->hosts);
}

const List<Host>* Mesh::getHosts() {
  return this->hosts;
}

void Mesh::scan() {
  MDNS.queryService("ssm", "tcp");

  uint16_t i = 0;
  while(true) {
    String h = MDNS.hostname(i); // FIXME Causes an error log on the serial.

    if(h == "") break;

    String ip = MDNS.IP(i).toString();

    Host host = Host(h, ip, MDNS.port(i));

    if(!contains(this->hosts, host)) {
      this->addHost(host);
    }

    i++;
  }
}

void Mesh::update() {
#ifdef ESP8266
  MDNS.update();
#endif
}

bool Mesh::begin(String hostname, String ip, uint16_t port) {
  // NOTE Add self to start the mesh.
  this->addHost(Host(hostname, ip, port));

  bool ret = MDNS.begin(hostname.c_str());
  MDNS.addService("http", "tcp", port);
  MDNS.addService("ssm", "tcp", port);

  return ret;
}
