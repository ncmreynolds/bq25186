/*
 * This sketch shows the system regulated voltage of the BQ25186 every 10 seconds.
 *
 * Assumes I2C connected on the default SDA/SCL. On the ESP32C3 used for testing this is...
 *
 * SDA 8
 * SCL 9
 *
 */

#include "Wire.h" //Include the I²C library
#include <bq25186.h>  //Include the BQ25186 library

bq25186 charger;  //Create a new instance of the charger object
uint32_t loopTimer = 0; //Use a loop timer instead of delay

void setup() {
  Serial.begin(115200);     //Set up the Serial for output
  while(!Serial){}          //Wait for Serial to start, only needed on some boards
  delay(5000);              //Give a USB connection time to come up
  //charger.debug(Serial);  //Enable (quite verbose) debug output for the charger. Necessary for print_registers() to work
  Wire.begin();             //Start I²C
  if(charger.begin()) {     //Start the charger
    Serial.println("Read charger configuration OK");
    if(charger.set_sys_regulation_voltage(BQ25186_SYS_REG_CTRL_4_5V)) { //This is the default, other sensible options are BQ25186_SYS_REG_CTRL_4_9V, BQ25186_SYS_REG_CTRL_PASS_THROUGH or BQ25186_SYS_REG_CTRL_BATTERY_TRACK note this will be the battery voltage plus 0.3v
      Serial.println("Set system regulated voltage");
    } else {
      Serial.println("Unable to set system regulated voltage");
    }
  } else {
    Serial.println("Unable to read charger registers, is it connected?");
  }
}

void loop() {
  if(millis() - loopTimer > 10e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    uint8_t sys_regulation_voltage = charger.get_sys_regulation_voltage();
    Serial.print("System regulated voltage:");
    if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_4_4V) {
      Serial.println("4.4v");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_4_5V) {
      Serial.println("4.5v");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_4_6V) {
      Serial.println("4.6v");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_4_7V) {
      Serial.println("4.7v");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_4_8V) {
      Serial.println("4.6v");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_4_9V) {
      Serial.println("4.9v");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_PASS_THROUGH) {
      Serial.println("pass through");
    } else if(sys_regulation_voltage == BQ25186_SYS_REG_CTRL_BATTERY_TRACK) {
      Serial.println("track battery");
    } else {
      Serial.println("unknown/error");
    }
  }
}
