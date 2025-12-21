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
  Serial.begin(115200);   //Set up the Serial for outpur
  while(!Serial){}        //Wait for Serial to start, only needed on some boards
  //charger.debug(Serial);  //Enable (quite verbose) debug output for the charger. Necessary for print_registers() to work
  Wire.begin();           //Start I²C
  charger.begin();        //Start the charger
}

void loop() {
  if(millis() - loopTimer > 10e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    //Power good
    Serial.print("\r\nPower in good:");
    printTrueFalse(charger.vin_pgood());
    Serial.print("\t");
    //VIN monitoring
    Serial.print("Charging voltage low threshold:");
    if(charger.get_vindpm() == BQ25186_VINDPM_DISABLED) {
      Serial.print("disabled");
    } else if(charger.get_vindpm() == BQ25186_VINDPM_4_7) {
      Serial.print("4.7v");
    } else if(charger.get_vindpm() == BQ25186_VINDPM_4_5) {
      Serial.print("4.5v");
    } else if(charger.get_vindpm() == BQ25186_VINDPM_VBAT_PLUS_300) {
      Serial.print("battery+300mV");
    }
    Serial.print("\t");
    //Charging enabled
    Serial.print("Charging enabled:");
    printTrueFalse(charger.get_chg_dis()==false); //This is inverted because it's a 'disabled' flag
    Serial.print("\t");
    //Charging
    Serial.print("Charging state:");
    if(charger.chg() == BQ25186_ENABLED_BUT_NOT_CHARGING) {
      Serial.println("Enabled but not charging");
    } else if(charger.chg() == BQ25186_CC_CHARGING) {
      Serial.println("Constant current charging");
    } else if(charger.chg() == BQ25186_CV_CHARGING) {
      Serial.println("Constant voltage charging");
    } else if(charger.chg() == BQ25186_CHARGING_DONE_OR_DISABLED) {
      Serial.println("Done or disabled");
    }
    //Cell voltage
    Serial.print("Cell regulated voltage:");
    Serial.print(charger.get_vbatreg());
    Serial.print("v\t");
    //Charging current
    Serial.print("Cell charge current:");
    Serial.print(charger.get_ichg());
    Serial.print("mA\t");
    //Termination current
    Serial.print("Cell termination charge current:");
    if(charger.get_iterm() == BQ25186_ITERM_20_PERCENT) {
      Serial.print("20%\t");
    } else if(charger.get_iterm() == BQ25186_ITERM_10_PERCENT) {
      Serial.print("10%\t");
    } else if(charger.get_iterm() == BQ25186_ITERM_5_PERCENT) {
      Serial.print("5%\t");
    } else if(charger.get_iterm() == BQ25186_ITERM_DISABLE) {
      Serial.print("disabled\t");
    }
    //Fast charge mode
    Serial.print("Fast charge mode:");
    printTrueFalse(charger.get_en_fc_mode());
    Serial.print("\t");
    //Discharge current limit
    Serial.print("Battery discharge over current protection limit:");
    Serial.print(charger.get_ibat_ocp());
    Serial.println("mA\t");
    //Battery undervoltage lockout threshold
    Serial.print("Battery undervoltage lockout threshold:");
    Serial.print(charger.get_buvlo());
    Serial.print("v\t");
    //Button actions
    Serial.print("Button long press time:");
    Serial.print(charger.get_long_press_time());
    Serial.print("s\t");
    Serial.print("Button long press action:");
    if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_NOTHING) {
      Serial.print("nothing\t");
    } else if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_RESET) {
      Serial.print("reset\t");
    } else if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_SHIP) {
      Serial.print("ship mode\t");
    } else if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_SHUTDOWN) {
      Serial.print("shutdown mode\t");
    }
    Serial.println();
    //System voltage regulation
    Serial.print("System regulated voltage:");
    if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_BATTERY_TRACK) {
      Serial.print("track battery\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_4_4V) {
      Serial.print("4.4V\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_4_5V) {
      Serial.print("4.5V\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_4_6V) {
      Serial.print("4.6V\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_4_7V) {
      Serial.print("4.7V\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_4_8V) {
      Serial.print("4.8V\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_4_9V) {
      Serial.print("4.9V\t");
    } else if(charger.get_sys_regulation_voltgage() == BQ25186_SYS_REG_CTRL_PASS_THROUGH) {
      Serial.print("pass through\t");
    }
    //System mode
    Serial.print("System power mode:");
    if(charger.get_sys_mode() == BQ25186_SYS_MODE_VIN_OR_VBAT) {
      Serial.print("Charger input or cell\t");
    } else if(charger.get_sys_mode() == BQ25186_SYS_MODE_VBAT_ONLY) {
      Serial.print("cell only\t");
    } else if(charger.get_sys_mode() == BQ25186_SYS_MODE_FLOAT) {
      Serial.print("disconnected and floating\t");
    } else if(charger.get_sys_mode() == BQ25186_SYS_MODE_PULLDOWN) {
      Serial.print("disconnected and pulled down\t");
    }
    //I2C watchdog
    Serial.print("I2C system watchdog:");
    if(charger.get_i2c_watchdog_mode() == BQ25186_SYS_WATCHDOG_15S_ENABLE) {
      Serial.println("enabled\t");
    } else {
      Serial.println("disabled\t");
    }
    Serial.println();
  }
}
void printTrueFalse(bool value) {
  if(value) {
    Serial.print("true ");
  } else {
    Serial.print("false");
  }
}
void printTrueFalseLn(bool value) {
  printTrueFalse(value);
  Serial.println();
}