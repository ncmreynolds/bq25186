/*
 * This sketch sets up ship mode and enters it after 60s, or if you press and hold the button for 5s then release it
 * 
 * Ship mode only works when powered by battery, if you have a supply at VIN, the BQ25186 will not enter ship mode
 *
 * Holding the button will wake it from ship mode
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
  //charger.debug(Serial);    //Enable (quite verbose) debug output for the charger. Necessary for print_registers() to work
  Wire.begin();             //Start I²C
  if(charger.begin()) {     //Start the charger
    Serial.println("Read charger configuration OK");
    if(charger.set_long_press_time(BQ25186_MR_LPRESS_5S)) { //Set to 5s
      Serial.println("Set button long press time");
    } else {
      Serial.println("Unable to set button long press time");
    }
    if(charger.set_long_press_action(BQ25186_PB_LPRESS_ACTION_SHIP)) {  //Set to enter ship mode
      Serial.println("Set button long press action");
    } else {
      Serial.println("Unable to set button long press action");
    }
  } else {
    Serial.println("Unable to read charger registers, is it connected?");
  }
}

void loop() {
  if(millis() - loopTimer > 1e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    if(millis() < 60e3) {
      Serial.print("Entering ship mode in ");
      Serial.print(int((60e3 - millis())/1e3));
      Serial.println("s pressing button will wake from ship");
    } else {
      Serial.print("Entering ship mode now:");
      if(charger.set_reset_ship(BQ25186_SHIP_MODE)) {
        Serial.println("OK"); //Realistically this won't be reached
      } else {
        Serial.println("failed");
      }
    }
  }
}
