# NodeMCU (ESP8266) RESTful Light Control

Quick and dirty implementation of RESTful light switch based on ESP8266 board

⚠ CAUTION: this project is purely for EDUCATION purposes, if you came here looking for ways to integrate your ESP8266 device into smart home ecosystem please take a look on following great projects:
- [ESPurna](https://github.com/xoseperez/espurna)
- [ESPHome](https://esphome.io/)

⚠ It's pretty much WIP, expect bugs, dirty code, lack of documentation

⚠ Requires a bit of soldering


## Description

Let's say you have a regular light source powered by USB cable. This code allows you to convert it to a smart light source which you can control via RESTful API.
Using RESTful you can potentially integrate your light with smart home systems. 
See Home Assistant configuration example below.

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

### Scheme

* Connect `D2` pin to the transitor base leg via 2k resistor.
* Connect `Vin` pin to USB Receptacle (+)
* Connect USB Receptacle (-) to the transitor collector leg
* Connect the transitor collector leg to `GND` pin


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