#include "UART.hpp"

UART* UART::create(JSONVar &config) {
  if (!config.hasOwnProperty("bus")) {
    return nullptr;
  }

  String bus = (const char *) config["bus"];

  if (bus == "software-uart") {
    if (!config.hasOwnProperty("rx") || !config.hasOwnProperty("tx")) {
      return nullptr;
    }

    uint8_t rx = (int) config["rx"];
    uint8_t tx = (int) config["tx"];

    return new SoftwareUART(rx, tx);
  } else if (bus == "hardware-uart") {
    if (!config.hasOwnProperty("number")) {
      return nullptr;
    }

    uint8_t number = (int) config["number"];

    switch (number) {
      case 0:
        return new HardwareUART(Serial);
      case 1:
        return new HardwareUART(Serial1);
#ifdef ESP32
      case 2:
        return new HardwareUART(Serial2);
#endif
      default:
        return nullptr;
    }
  } else {
    return nullptr;
  }
}
