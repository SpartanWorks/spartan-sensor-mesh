# Spartan Sensor Mesh

This project uses several connected ESP8266 boards to display various sensor readings in a clean and configurable way.
[Here's](http://idorobots.github.io/spartan-sensor-mesh/) a live demo of the UI, and [some](./landscape.png) [images](./portrait.png).

## Building

The frontend is a run-of-the-mill TypeScript app. To build it:

```
npm i
npm run build:release
```

It will update some static files on the backend side to make them ready for upload.

The backend is organized as a PlatformIO project with two targets: ESP32 and ESP8266. To build the backend run the following commands:

```
pio run -t esp32dev # Or pio run -t esp12e
pio run -t uploadfs
pio run -t upload
```

## Configuration

The backend expects two configuration files to be present in its filesystem:

- `wifi_config.txt` - optional, contains the SSID and password (on separate two lines) of the network that it should connect to. If not provided, it'll start an access point and allow reconfiguring the network via the UI.

- `device_config.json` - required, a configuration describing what sensors are hooked up to the module:

```
{
  "model": string,
  "group": string,
  "name": string,
  "password": string (password for the AP and the configuration part of the UI),
  "sensors": [
    {
      "type": string (names the sensor to use),
      "enabled": bool (whether to enable this sensor or not),
      "connection": {
        "bus": string (depends on the sensor),
        "other properties": any (arguments for the bus, depends on the sensor)
      },
      "capabilities": [
        {
          "reading": string (type of supported reading),
          "name": string (name of the reading to use)
        }
      ],
      "other properties": any (any extra arguments to configure the sensor, such as precission, etc)
    }
  ]
}
```

## Development

You can run the frontend locally against a sensor in the following way:

```
SERVER="http://address.of.server:port" npm run build:watch
```

Then your frontend will be avaliable under http://localhost:8888 and pointing to `$SERVER` with automatic reload of the code changes and sourcemaps in place.

The backend can be monitored by hooking it up with a serial cable, and running the serial monitor:

```
pio run -t monitor
```
