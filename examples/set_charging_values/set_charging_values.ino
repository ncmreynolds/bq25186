/*
 * This sketch shows the status of the BQ25186 every 10 seconds.
 *
 * Assumes I2C connected on the default SDA/SCL. On the ESP32C3 used for testing this is...
 *
 * SDA 8
 * SCL 9
 *
 *
 *
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
  //charger.debug(Serial);    //Enable (quite verbose) debug output for the charger. Necessary for print_registers() to work
  Wire.begin();             //Start I²C
  if(charger.begin()) {     //Start the charger
    Serial.println("Read charger configuration OK");
    if(charger.set_ichg(200)) { //Set to 200mA
      Serial.println("Set maximum charging current");
    } else {
      Serial.println("Unable to set maximum charging current");
    }
    if(charger.set_vbatreg(4.2)) {  //Set to 4.2V
      Serial.println("Set battery regulation voltage");
    } else {
      Serial.println("Unable to set battery regulation voltage");
    }
    if(charger.set_iterm(BQ25186_ITERM_10_PERCENT)) { //Set termination current to 10% (default) other reasonable options are BQ25186_ITERM_5_PERCENT BQ25186_ITERM_20_PERCENT
      Serial.println("Set battery charge termination current");
    } else {
      Serial.println("Unable to set battery charge termination current");
    }
  } else {
    Serial.println("Unable to read charger registers, is it connected?");
  }
}

void loop() {
  if(millis() - loopTimer > 10e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    Serial.print("Charging state:");
    if(charger.chg_stat() == BQ25186_ENABLED_BUT_NOT_CHARGING) {
      Serial.println("enabled but not charging");
    } else if(charger.chg_stat() == BQ25186_CC_CHARGING) {
      Serial.print("constant current:");
      Serial.print(charger.get_ichg());
      Serial.println("mA");
    } else if(charger.chg_stat() == BQ25186_CV_CHARGING) {
      Serial.print("constant voltage:");
      Serial.print(charger.get_vbatreg());
      Serial.print("v, termination limit: ");
      if(charger.get_iterm() == BQ25186_ITERM_5_PERCENT) {
        Serial.print(charger.get_ichg() * 0.05);
        Serial.println("mA");
      } else if(charger.get_iterm() == BQ25186_ITERM_10_PERCENT) {
        Serial.print(charger.get_ichg() * 0.1);
        Serial.println("mA");
      } else if(charger.get_iterm() == BQ25186_ITERM_20_PERCENT) {
        Serial.print(charger.get_ichg() * 0.2);
        Serial.println("mA");
      } else if(charger.get_iterm() == BQ25186_ITERM_DISABLE) {
        Serial.println("disabled");
      } else {
        Serial.println("unknown");
      }
    } else if(charger.chg_stat() == BQ25186_CHARGING_DONE_OR_DISABLED) {
      Serial.println("done or disabled");
    } else {
      Serial.println("unknown");
    }
  }
}
