/*
 * Simple monitoring of power and battery status
 *
 * Assumes some very common charge parameters and I2C connected on the default SDA/SCL on the ESP32C3 used for testing this is...
 *
 * SDA 8
 * SCL 9
 *
 *
 *
 *
*/

#include "Wire.h"
#include <bq25186.h>

bq25186 charger;
uint32_t loopTimer = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  Wire.begin();
  charger.begin();
  charger.debug(Serial);
}

void loop() {
  if(millis() - loopTimer > 1e3) {
    loopTimer = millis();
    Serial.println("Tick");
  }
}
