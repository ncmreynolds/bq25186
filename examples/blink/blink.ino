/*
 * This sketch blinks the LED connected to the BQ25186 on the PG pin showing its use as an indicator
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
bool ledState = true;  //Track LED state

void setup() {
  Serial.begin(115200);     //Set up the Serial for outpur
  while(!Serial){}          //Wait for Serial to start, only needed on some boards
  //charger.debug(Serial);  //Enable (quite verbose) debug output for the charger. Necessary for print_registers() to work
  delay(5000);              //Give a USB connection time to come up
  Wire.begin();             //Start I²C
  if(charger.begin()) {     //Start the charger
    Serial.println("Read charger configuration OK");
    if(charger.set_pg_pin_mode(BQ25186_PG_PIN_MODE_GPO)) {
      Serial.println("PG Pin set as output");
    } else {
      Serial.println("Unable to set PG Pin as output");
    }
  } else {
    Serial.println("Unable to read charger registers, is it connected?");
  }
}

void loop() {
  if(millis() - loopTimer > 1e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    Serial.print("LED ");
    if(ledState == true) {
      ledState = false;
      if(charger.set_pg_pin_state(BQ25186_PG_GPO_HIGH_IMPEDENCE)) {
        Serial.println("Off");
      } else {
        Serial.println("Unable to set PG Pin high impedence");
      }
    } else {
      ledState = true;
      if(charger.set_pg_pin_state(BQ25186_PG_GPO_LOW)) {
        Serial.println("On");
      } else {
        Serial.println("Unable to set PG Pin low");
      }
    }
  }
}
