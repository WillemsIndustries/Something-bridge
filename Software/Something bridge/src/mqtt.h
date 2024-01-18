#define mqtt_user "something bridge"
#define mqtt_password "qxHrJUAZ9i"
int mqtt_time;
bool HA_online = false;

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void callback(char* topic, byte* payload, unsigned int length);
void mqtt_auto_discovery(String name);

// setup MQTT
void mqtt_init(){
    // reading mqtt server address
    String HA_address = eeprom_read_string(192);
    static char mqtt_server[64] = {0};
    strcpy(mqtt_server, HA_address.c_str());
    // set MQTT domain/ip and port
    mqtt_client.setServer(mqtt_server, 1883);
    mqtt_client.setCallback(callback);
    mqtt_client.setBufferSize(1024);
}
// Try's to connect to the Homeassistant MQTT server every 5 seconds
void mqtt_connect(){
    if(millis() - mqtt_time >= 5000){
        mqtt_time = millis();
        flash_led(status_led, 2);
        mqtt_client.connect(device_name, mqtt_user, mqtt_password);
        if (mqtt_client.connected()){
            digitalWrite(status_led, HIGH);
            Serial.println("MQTT connected");
            mqtt_auto_discovery("shutter_group_1");
            mqtt_auto_discovery("shutter_group_2");
            mqtt_auto_discovery("shutter_group_3");
            mqtt_auto_discovery("shutter_group_4");
            mqtt_auto_discovery("shutter_group_5");
            mqtt_client.subscribe("homeassistant/status");
        }
    }
}

// send a discovery massage to home assistant
void mqtt_auto_discovery(String name){
    String str_payload;
    StaticJsonDocument<600> payload;
    JsonObject device;
    JsonArray identifiers;
    payload["name"] = name;
    payload["device_class"] = "shutter";
    payload["unique_id"] = name;
    payload["command_topic"] = "rts-gateway/" + name + "/set";
    payload["payload_open"] = "up";
    payload["payload_close"] = "down";
    payload["payload_stop"] = "my";
    device = payload.createNestedObject("device");
    device["name"] = device_name;
    device["model"] = device_name;
    device["sw_version"] = software_version;
    device["manufacturer"] = "Something Inc.";
    identifiers = device.createNestedArray("identifiers");
    identifiers.add(serial_number);
    serializeJson(payload, str_payload);
    mqtt_client.publish(("homeassistant/cover/" + name + "/config").c_str(), str_payload.c_str());
    mqtt_client.subscribe(("rts-gateway/" + name + "/set").c_str());
}

// callback for MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    String str_topic = topic;
    payload[length] = '\0';
    String str_payload = String((char*)payload);
    if (str_topic == "rts-gateway/shutter_group_1/set") {
        shutter(1, str_payload);
    } else if (str_topic == "rts-gateway/shutter_group_2/set") {
        shutter(2, str_payload);
    } else if (str_topic == "rts-gateway/shutter_group_3/set") {
        shutter(3, str_payload);
    } else if (str_topic == "rts-gateway/shutter_group_4/set") {
        shutter(4, str_payload);
    } else if (str_topic == "rts-gateway/shutter_group_5/set") {
        shutter(5, str_payload);
    } else if (str_topic == "homeassistant/status") {
        if(str_payload == "online"){
            mqtt_client.disconnect();
        }
    }
}