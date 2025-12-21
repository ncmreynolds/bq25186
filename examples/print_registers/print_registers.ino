/*
 * This script dumps the registers of the BQ25186 every second. You should refer to the datasheet section 6.5 (https://www.ti.com/product/BQ25186) to interpret them
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
  Serial.begin(115200);   //Set up the Serial for outpur
  while(!Serial){}        //Wait for Serial to start, only needed on some boards
  charger.debug(Serial);  //Enable (quite verbose) debug output for the charger. Necessary for print_registers() to work
  Wire.setClock(100e3);
  Wire.begin();           //Start I²C
  charger.begin();        //Start the charger
}

void loop() {
  if(millis() - loopTimer > 10e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    charger.print_registers();      //Print all the registers. Note this implies a read before printing them but it is rate limited to once every 100ms!
  }
}