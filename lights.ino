#include <ESP8266WiFi.h>
#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50
#define TRANSITION 1000

const int     LED_ao = 4;
unsigned long previousMillis = 0;
bool          new_state = false;
float         trans_step = 0;
unsigned long currentMillis = millis();
unsigned long privMillis = 0;

WiFiManager      wifiManager;
ESP8266WebServer http_rest_server(HTTP_REST_PORT);


struct Light {
  float brightness;
  float target_brightness;
  bool light_state;
  unsigned long trans_timer;
} light;

void init_light_resource()
{
  light.brightness = 0;
  light.target_brightness = 0;
  light.light_state = false;
  light.trans_timer = 0;
}

void get_light() {
  StaticJsonDocument<200> jsonObj;
  char JSONmessageBuffer[200];
  jsonObj["brightness"] = light.brightness;
  jsonObj["light_state"] = light.light_state;
  jsonObj["trans_timer"] = light.trans_timer;
  serializeJson(jsonObj, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void post_put_light() {
  StaticJsonDocument<500> jsonBody;
  String post_body = http_rest_server.arg("plain");
  Serial.println(post_body);

  DeserializationError error = deserializeJson(jsonBody, http_rest_server.arg("plain"));

  Serial.print("HTTP Method: ");
  Serial.println(http_rest_server.method());

  if (error) {
    Serial.println("error in parsin json body");
    http_rest_server.send(400);
  }
  else {
    if (http_rest_server.method() == HTTP_PUT) {
      json_to_resource(jsonBody);
      http_rest_server.send(200);
    }
  }
}

void start_transition(int timer) {
  light.trans_timer = millis() + timer;
  trans_step = (light.target_brightness - light.brightness) / float(timer);
  privMillis = 0;
}

void json_to_resource(StaticJsonDocument<500> jsonBody) {
  light.target_brightness = jsonBody["brightness"];
  light.light_state = jsonBody["light_state"];
  if (int(jsonBody["night_timer"]) > 0) {
    start_transition(int(jsonBody["night_timer"]) * 1000);
  } else {
    light.trans_timer = 0;
  }
  new_state = true;
}


void config_rest_server_routing() {
  http_rest_server.on("/", HTTP_GET, []() {
    http_rest_server.send(200, "text/html",
                          "Welcome to the ESP8266 REST Web Server");
  });
  http_rest_server.on("/leds", HTTP_GET, get_light);
  http_rest_server.on("/leds", HTTP_PUT, post_put_light);
}

void set_light() {
  if (light.target_brightness == light.brightness) {
    light.trans_timer = 0;
    privMillis = 0;
  }

  if (light.trans_timer == 0 && light.brightness != light.target_brightness) {
    start_transition(TRANSITION);
  }

  if (light.brightness == 0) {
    light.light_state = false;
  }

  if (light.target_brightness != light.brightness) {
    light.light_state = true;
  }

  if (light.light_state) {
    analogWrite(LED_ao, round(light.brightness));
  } else {
    analogWrite(LED_ao, 0);
  }
  new_state = false;
}

void setup(void) {
  Serial.begin(115200);
  wifiManager.autoConnect();
  init_light_resource();
  config_rest_server_routing();
  http_rest_server.begin();
  Serial.println("HTTP REST Server Started");
  privMillis = 0;
}

void loop(void) {
  http_rest_server.handleClient();
  if (light.trans_timer > 0) {
    if (privMillis == 0) {
      privMillis = millis();
    }
    currentMillis = millis();
    if (light.trans_timer > currentMillis) {
      light.brightness = light.brightness + trans_step * (currentMillis - privMillis);
      privMillis = millis();
    } else {
      light.brightness = light.target_brightness;
    }
    new_state = true;
  }

  if (new_state) {
    set_light();
  }
  delay(1);
}
