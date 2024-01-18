// hotspot IP configuration
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
// wifi
int wifi_time;
bool wifi_connected = false;
// webserver
ESP8266WebServer server(80);
void get_index();
void get_style();
void get_javascript();
void get_logo();
void save_data();
void load_data();
void search_networks();

// setup en start wifi
void wifi_init(){
    String wifi_SSID = eeprom_read_string(64);
    String wifi_password = eeprom_read_string(128);
    WiFi.hostname("something-bridge");
    WiFi.begin(wifi_SSID, wifi_password);
}
// wait for wifi connection
void wifi_connect(){
    wifi_connected = false;
    if(millis() - wifi_time >= 1500){
        wifi_time = millis();
        flash_led(status_led, 1);
    }
    if(WiFi.status() == WL_CONNECTED){
        Serial.println("wifi connected to " + WiFi.SSID());
        Serial.print("ip address: ");
        Serial.println(WiFi.localIP());
        wifi_connected = true;
    }
}
// starts and maintains the hotspot and webserver for configuring the settings.
void config_modes(){
    Serial.println("starting configuration");
    // setup hotspot
    WiFi.softAP(device_name);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    // setup webserver
    server.on("/", HTTP_GET, get_index);
    server.on("/static/style.css", HTTP_GET, get_style);
    server.on("/static/index.js", HTTP_GET, get_javascript);
    server.on("/static/logo.png", HTTP_GET, get_logo);
    server.on("/save", HTTP_GET, save_data);
    server.on("/load", HTTP_GET, load_data);
    server.on("/networks", HTTP_GET, search_networks);
    server.begin();
    digitalWrite(status_led, HIGH);
    digitalWrite(pair_led, HIGH);
    // webserver loop
    while(true){
        server.handleClient();
        reset_button_handler();
    }
}
/*========== EEPROM ==========*/
// write to the eeprom
void eeprom_write_string(int addr_offset, const String &text){
  byte len = text.length();
  EEPROM.write(addr_offset, len);
  for (int i = 0; i < len; i++){
    EEPROM.write(addr_offset + 1 + i, text[i]);
  }
  EEPROM.commit();
}
// read from the eeprom
String eeprom_read_string(int addr_offset){
  int len = EEPROM.read(addr_offset);
  char data[len + 1];
  for (int i = 0; i < len; i++){
    data[i] = EEPROM.read(addr_offset + 1 + i);
  }
  data[len] = '\0';
  return String(data);
}

/*========== WEBSERVER FILES ==========*/
void get_index(){
    LittleFS.begin();
    File file = LittleFS.open("templates/index.html", "r");
    server.send(200, "text/html", file);
    LittleFS.end();
}

void save_data(){
    String wifi_SSID = server.arg("wifi_SSID");
    String wifi_password = server.arg("wifi_password");
    String HA_address = server.arg("HA_address");
    Serial.println(wifi_SSID);
    eeprom_write_string(64, wifi_SSID);
    Serial.println(wifi_password);
    eeprom_write_string(128, wifi_password);
    Serial.println(HA_address);
    eeprom_write_string(192, HA_address);
    server.send(200, "text/html", "");
    ESP.restart();
}

void load_data(){
    String wifi_SSID = eeprom_read_string(64);
    String wifi_password = eeprom_read_string(128);
    String HA_address = eeprom_read_string(192);
    server.send(200, "text/html", "{\"" + wifi_SSID + "\"},{\"" + wifi_password + "\"},{\"" + HA_address + "\"}");
}

void search_networks(){
    int num_networks = WiFi.scanNetworks();
    String ssid[num_networks];
    String data = "{\"";
    for(int i = 0; i < num_networks; i++){
        bool exists = false;
        for(int x = 0; x < num_networks; x++){
            if(ssid[x] == WiFi.SSID(i)){
                exists = true;
            }
        }
        if(!exists && i == num_networks - 1){
            data = data + WiFi.SSID(i) + "\"}";
        } else if(!exists) {
            data = data + WiFi.SSID(i) + "\"},{\"";
        } else if(exists && i == num_networks - 1) {
            data.remove(data.length()-3, 3);
        }
        ssid[i] = WiFi.SSID(i);
    }
    server.send(200, "text/html", data);
}

void get_style(){
    LittleFS.begin();
    File file = LittleFS.open("static/style.css", "r");
    server.send(200, "text/css", file);
    LittleFS.end();
}

void get_javascript(){
    LittleFS.begin();
    File file = LittleFS.open("static/index.js", "r");
    server.send(200, "text/js", file);
    LittleFS.end();
}

void get_logo(){
    LittleFS.begin();
    File file = LittleFS.open("static/logo.png", "r");
    server.send(200, "image", file);
    LittleFS.end();
}