/*
Something bridge
Software for controlling Somfy RTS shutters with Home Assistant.
Bram en Richard
2023-2024
*/
// Device parameters
#define device_name "Something bridge"
#define software_version "1.0.2"
#define serial_number "0001"
// Libaries
#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
// local files
#include "hardware.h"
#include "wifi.h"
#include "rts_remotes.h"
#include "mqtt.h"
// main setup
void setup(){
    Serial.begin(115200);
    Serial.println("");
    Serial.println(device_name);
    Serial.print("software version: ");
    Serial.println(software_version);
    Serial.print("serial_number: ");
    Serial.println(serial_number);
    EEPROM.begin(512);
    pins_init();
    if (!digitalRead(pair_button.pin)){
        config_modes();
    }
    wifi_init();
    mqtt_init();
}
// main loop
void loop(){
    if ((WiFi.status() != WL_CONNECTED && !wifi_connected) || WiFi.status() != WL_CONNECTED || !wifi_connected){
        wifi_connect();
    } else if (!mqtt_client.connected()){
        mqtt_connect();
    } else {
        mqtt_client.loop();
        pair_button_handler();
    }
    reset_button_handler();
}