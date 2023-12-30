# Spartan Sensor Mesh

This project uses several connected agents (ESP8266/ESP32 devices or JVM agents) to display various readings in a clean and configurable way.
[Here's](http://ssn.idorobots.org) a live demo of the UI, and [some](./landscape.png) [images](./portrait.png).

## Building

The `ui` is a run-of-the-mill TypeScript app. To build it:

```
npm i
npm run build:release
```

It will update some static files on the backend side to make them ready for upload.

The `device` is organized as a PlatformIO project with two targets: ESP32 and ESP8266. To build the `device` backend run the following commands:

```
pio run -t esp32dev # Or pio run -t esp12e
pio run -t uploadfs
pio run -t upload
```

The `agent` is a JVM-based variant of the sensing service, currently also supporting GraalVM native-image compilation & Docker. To build the `agent` backend run the following commands:

```
sbt assembly # Creates a runnable uberjar.
sbt Docker/publishLocal # Creates a runnable container image.
sbt GraalVMNativeImage/packageBin # Creates a native binary for your system.
```

You can run the Agent Docker image with the following command. Put your agent config JSON in the ./data directory, along with optional UI assets.

```
docker run --network host -v ./data:/opt/docker/data -it spartan-sensor-mesh:0.1.0-SNAPSHOT
```

## Configuration

The `device` backend expects two configuration files to be present in its filesystem:

- `wifi_config.txt` - optional, contains the SSID and password (on separate two lines) of the network that it should connect to. If not provided, it'll start an access point and allow reconfiguring the network via the UI.

- `device_config.json` - required, a configuration describing what sensors are hooked up to the module:

```
{
  "model": string,
  "group": string,
  "name": string,
  "password": string (password for the AP and the configuration part of the UI),
  "log": {
    "level": string (one of: debug, info, warning, error, critical),
    "console": {
      "bus": string (should be one of the available UART busses),
      "other properties": any (arguments for the bus, depends on the bus)
    }
  },
  "sensors": [
    {
      "type": string (names the sensor to use),
      "enabled": bool (whether to enable this sensor or not),
      "connection": {
        "bus": string (depends on the sensor),
        "other properties": any (arguments for the bus, depends on the sensor)
      },
      "readings": [
        {
          "type": string (type of supported reading),
          "name": string (name of the reading to use),
          "averaging": number,
          "widget": json (display widget specific configuration, used to guide the UI)
        }
      ],
      "other properties": any (any extra arguments to configure the sensor, such as precission, etc)
    }
  ]
}
```

## Development

You can run the UI locally against a sensor in the following way:

```
SERVER="http://address.of.server:port" npm run build:watch
```

Then the UI will be avaliable under http://localhost:8888 and pointing to `$SERVER` with automatic reload of the code changes and sourcemaps in place.

The `device` backend can be monitored by hooking it up with a serial cable, and running the serial monitor:

```
pio run -t monitor
```

The `agent` backend can be run in debug mode (ideally via a language server protocol implementation), and utilizes the following extra commands for convenience:

```
sbt scalafm
sbt sacalfixAll
sbt coverage test coverageReport
```
