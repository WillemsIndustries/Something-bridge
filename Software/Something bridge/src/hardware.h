// button structure
struct BUTTON {
  const uint8_t pin;
  int key_presses;
  bool pressed;
  int press_time;
  int release_time;
};

// Buttons {pin number, number of key presses, is pressed, time when pressed, time when released}
BUTTON pair_button = {14, 0, false, 0, 0};
BUTTON reset_button = {5, 0, false, 0, 0};
// Led pins
#define status_led 12
#define pair_led 13
// Antenna pin
#define rts_data_pin 4

// 12ns delay for hardware interrupts
#define NOP __asm__ __volatile__ ("nop")

void IRAM_ATTR pair_interrupt();
void IRAM_ATTR reset_interrupt();
void flash_led(int led_pin, int times);
void shutter(int remote, String command);
void eeprom_write_string(int addr_offset, const String &text);
String eeprom_read_string(int addr_offset);

// sets the pinmodes and attaches the interrupts
void pins_init(){
    pinMode(status_led, OUTPUT);
    digitalWrite(status_led, LOW);
    pinMode(pair_led, OUTPUT);
    digitalWrite(pair_led, LOW);
    pinMode(rts_data_pin, OUTPUT);
	digitalWrite(rts_data_pin, LOW);
    pinMode(pair_button.pin, INPUT);
    pinMode(reset_button.pin, INPUT);
    // interrupts
    attachInterrupt(digitalPinToInterrupt(pair_button.pin), pair_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(reset_button.pin), reset_interrupt, CHANGE);
}
// interrupt function for the pair button
void IRAM_ATTR pair_interrupt(){
    // 2400ns delay to resolve the unreliable edge detection
    for (int delay=0; delay<200; delay++) NOP; 
    if(!digitalRead(pair_button.pin)){
        pair_button.press_time = millis();
        if(millis() - pair_button.release_time >= 2000){
            pair_button.key_presses = 1;
        }else if(pair_button.key_presses != 5){
            pair_button.key_presses++;
        }
    } else {
        pair_button.release_time = millis();
        pair_button.pressed = true;
    }
}

// interrupt function for the reset button
void IRAM_ATTR reset_interrupt(){
    // 2400ns delay to resolve the unreliable edge detection
    for (int delay = 0; delay < 200; delay++) NOP;
    if (!digitalRead(reset_button.pin)) {
        reset_button.press_time = millis();
    } else {
        reset_button.release_time = millis();
        reset_button.pressed = true;
    }
}

// handler for the pair button
void pair_button_handler(){
    if (pair_button.pressed){
            pair_button.pressed = false;
            if (pair_button.release_time - pair_button.press_time >= 2000){
                shutter(pair_button.key_presses, "prog");
                flash_led(pair_led, pair_button.key_presses);
            }
    }
}
// handler for the reset button
void reset_button_handler(){
    if (reset_button.pressed) {
        reset_button.pressed = false;
        if (reset_button.release_time - reset_button.press_time <= 2000) {
            ESP.restart();  
        } else if (reset_button.release_time - reset_button.press_time > 2000) {
        for (unsigned int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0);
        }
        eeprom_write_string(192, "homeassistant.local");
        flash_led(status_led, 1);
        EEPROM.commit();
        ESP.restart();
    }
    }
}

// smoothly blinks a LED for a specified number of times
// "led_pin" is the pin of the led you want to use
// "times" is the number of times you want the led to blink
void flash_led(int led_pin, int times){
    int t = 0;
    while(t < times){
        t = t + 1;
        for (int brightness = 0; brightness < 256; brightness++) {
            analogWrite(led_pin, brightness);
            delay(50.0/256);
        }
        for (int brightness = 0; brightness < 256; brightness++) {
            analogWrite(led_pin, 255-brightness);
            delay(50.0/256);
        }
        delay(150);
    }
}