#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266mDNS.h>
#endif

#ifdef ESP32
#include <ESPmDNS.h>
#endif

#include "List.hpp"

struct Host {
  String hostname;
  String ip;
  uint16_t port;

  Host(String h, String i, uint16_t p)
      : hostname(h),
        ip(i),
        port(p)
  {}

  bool operator==(const Host& other) const {
    return hostname == other.hostname && ip == other.ip && port == other.port;
  }
};

class Mesh {
 private:
  List<Host> *hosts; // FIXME Could use Reading<any> here.

 public:

  Mesh();
  ~Mesh();

  bool begin(String hostname, String ip, uint16_t port);

  void scan();
  void update();
  void addHost(Host h);

  const List<Host> *getHosts();
};

#endif
