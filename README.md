# NodeMCU (ESP8266) RESTful Light Control

Quick and dirty implementation of RESTful light swich based on ESP8266

⚠ It's pretty much WIP, expect bugs, dirty code, lack of documentation

⚠ Requires a bit of soldering


## Description

Let's say you have a regular light source powered by USB cable. This code allows you to convert it a smart light source which you can control via RESTful API.
Using RESTful you can potentially integrate your light with smart home systems. 
See Home Assistant configuration example.

Currently supported features:
- Turn ON/OFF
- Brightness
- Transition duration
- AP mode for WiFi configuration via WebUI

## Schematic

### BOM

1. NodeMCU (ESP8266)
2. ~2K ohm resistor
3. BC547 transistor
4. USB-A Receptacle Connector (female)


## Flashing

You can use one of following:
- https://github.com/igrr/esptool-ck
- Arduino IDE
- https://github.com/nodemcu/nodemcu-flasher

## Configuration

Be default it starts in AP WiFI mode. To connect it exiting network:
1. Connect to ESP-XXXXXX wifi network, where XXXXX is your chip id and different for each chip
2. Go to http://192.168.4.1 and configure wifi settings

## API

Base URL: http://192.168.x.x/leds

JSON Object: `{"brightness":1023,"light_state":true,"night_timer":0}`

where:
- brightness: float (0-1023)
- light_state: bool (indicates if the light is on or off)
- night_timer: int (transition duration in seconds)

Methods: 
- `GET` - returns JSON Object with current state
- `PUT` (Body: JSON Object) - sets light state. `light_state` value is ignored. Set `brightness` to `0` to turn the light off 

## Home Assistant integration

In `configuration.yaml` add following
 
```yaml
switch:
  - platform: rest
    name: NodeMCU Light
    resource: http://192.168.x.x/leds
    method: put
    body_on: '{"brightness":1023,"light_state":true,"night_timer":0}'
    body_off: '{"brightness":0,"light_state":false,"night_timer":0}'
    is_on_template: "{{ value_json.light_state }}"
    headers:
      Content-Type: application/json

light:
  - platform: switch
    name: NodeMCU Lights
    entity_id: switch.nodemcu_light
```

## Known Issues

- No brightness control in Home Assistant
- `night_timer` parameter should be renamed to something more sensible
- Default transition time (1s) is hardcoded
- Lack of configurable parameters
- `night_timer` returns incorrect value
- `light_state` is ignored