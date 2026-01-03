/*
 * This sketch shows the status of the BQ25186 every 10 seconds.
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
  } else {
    Serial.println("Unable to read charger registers, is it connected?");
  }
}

void loop() {
  if(millis() - loopTimer > 10e3) { //Query the charger every 10s
    loopTimer = millis();           //Avoiding using delay()
    //Power good
    Serial.print("\r\nCharging\r\nPower in good:");
    printTrueFalse(charger.vin_pgood_stat() == BQ25186_POWER_GOOD);
    Serial.print("\t");
    //VIN monitoring
    Serial.print("Charging VINDPM low threshold:");
    if(charger.get_vindpm() == BQ25186_VINDPM_DISABLED) {
      Serial.print("disabled");
    } else if(charger.get_vindpm() == BQ25186_VINDPM_4_7) {
      Serial.print("4.7V");
    } else if(charger.get_vindpm() == BQ25186_VINDPM_4_5) {
      Serial.print("4.5V");
    } else if(charger.get_vindpm() == BQ25186_VINDPM_VBAT_PLUS_300) {
      Serial.print("battery+300mV");
    } else {
      Serial.print("unknown");
    }
    Serial.print("\t");
    //Charging enabled
    Serial.print("Charging enabled:");
    printTrueFalse(charger.get_chg_dis()==false); //This is inverted because it's a 'disabled' flag
    Serial.print("\t");
    //Charging
    Serial.print("Charging state:");
    if(charger.chg_stat() == BQ25186_ENABLED_BUT_NOT_CHARGING) {
      Serial.println("Enabled but not charging");
    } else if(charger.chg_stat() == BQ25186_CC_CHARGING) {
      Serial.println("Constant current charging");
    } else if(charger.chg_stat() == BQ25186_CV_CHARGING) {
      Serial.println("Constant voltage charging");
    } else if(charger.chg_stat() == BQ25186_CHARGING_DONE_OR_DISABLED) {
      Serial.println("Done or disabled");
    } else {
      Serial.print("unknown");
    }
    //Battery voltage
    Serial.print("\r\nBattery\r\nRegulated voltage:");
    Serial.print(charger.get_vbatreg());
    Serial.print("v\t");
    //Charging current
    Serial.print("Charge current:");
    Serial.print(charger.get_ichg());
    Serial.print("mA\t");
    //Termination current
    Serial.print("Termination charge current:");
    if(charger.get_iterm() == BQ25186_ITERM_20_PERCENT) {
      Serial.print("20%");
    } else if(charger.get_iterm() == BQ25186_ITERM_10_PERCENT) {
      Serial.print("10%");
    } else if(charger.get_iterm() == BQ25186_ITERM_5_PERCENT) {
      Serial.print("5%");
    } else if(charger.get_iterm() == BQ25186_ITERM_DISABLE) {
      Serial.print("disabled");
    } else {
      Serial.print("unknown");
    }
    Serial.print("\t");
    //Fast charge mode
    Serial.print("Fast charge mode:");
    printEnabledDisabledLn(charger.get_en_fc_mode());
    //Discharge current limit
    Serial.print("\r\nProtection\r\nDischarge over current protection limit:");
    if(charger.get_ibat_ocp() == BQ25186_IBAT_OCP_500MA) {
      Serial.print(500);
    } else if(charger.get_ibat_ocp() == BQ25186_IBAT_OCP_1000MA) {
      Serial.print(1000);
    } else if(charger.get_ibat_ocp() == BQ25186_IBAT_OCP_1500MA) {
      Serial.print(1500);
    } else if(charger.get_ibat_ocp() == BQ25186_IBAT_OCP_3000MA) {
      Serial.print(3000);
    } else {
      Serial.print("unknown ");
    }
    Serial.print("mA\t");
    //Battery undervoltage lockout threshold
    Serial.print("Battery undervoltage lockout threshold:");
    Serial.print(charger.get_buvlo());
    Serial.println("v\t");
    //Button actions
    Serial.print("\r\nButton\r\nPush:");
    printEnabledDisabled(charger.get_en_push() == BQ25186_PUSH_ENABLED);
    Serial.print("\tLong press time:");
    if(charger.get_long_press_time() == BQ25186_MR_LPRESS_5S) {
      Serial.print(5);
    } else if(charger.get_long_press_time() == BQ25186_MR_LPRESS_10S) {
      Serial.print(10);
    } else if(charger.get_long_press_time() == BQ25186_MR_LPRESS_15S) {
      Serial.print(15);
    } else if(charger.get_long_press_time() == BQ25186_MR_LPRESS_20S) {
      Serial.print(20);
    } else {
      Serial.print("unknown ");
    }
    Serial.print("s\t");
    Serial.print("Long press action:");
    if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_NOTHING) {
      Serial.print("nothing");
    } else if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_RESET) {
      Serial.print("reset");
    } else if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_SHIP) {
      Serial.print("ship mode");
    } else if(charger.get_long_press_action() == BQ25186_PB_LPRESS_ACTION_SHUTDOWN) {
      Serial.print("shutdown mode");
    } else {
      Serial.print("unknown");
    }
    Serial.println();
    //System voltage regulation
    Serial.print("\r\nSystem\r\nRegulated voltage:");
    if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_BATTERY_TRACK) {
      Serial.print("track battery");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_4_4V) {
      Serial.print("4.4V");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_4_5V) {
      Serial.print("4.5V");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_4_6V) {
      Serial.print("4.6V");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_4_7V) {
      Serial.print("4.7V");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_4_8V) {
      Serial.print("4.8V");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_4_9V) {
      Serial.print("4.9V");
    } else if(charger.get_sys_regulation_voltage() == BQ25186_SYS_REG_CTRL_PASS_THROUGH) {
      Serial.print("pass through");
    } else {
      Serial.print("unknown");
    }
    Serial.print("\t");
    //System mode
    Serial.print("Power mode:");
    if(charger.get_sys_mode() == BQ25186_SYS_MODE_VIN_OR_VBAT) {
      Serial.print("Charger input or battery\t");
    } else if(charger.get_sys_mode() == BQ25186_SYS_MODE_VBAT_ONLY) {
      Serial.print("Battery only\t");
    } else if(charger.get_sys_mode() == BQ25186_SYS_MODE_FLOAT) {
      Serial.print("disconnected and floating\t");
    } else if(charger.get_sys_mode() == BQ25186_SYS_MODE_PULLDOWN) {
      Serial.print("disconnected and pulled down\t");
    } else {
      Serial.print("unknown");
    }
    //I2C watchdog
    Serial.print("I2C system startup watchdog:");
    printEnabledDisabledLn(charger.get_i2c_watchdog_mode() == BQ25186_SYS_WATCHDOG_15S_ENABLE);
  }
}
void printTrueFalse(bool value) {
  if(value) {
    Serial.print("true");
  } else {
    Serial.print("false");
  }
}
void printTrueFalseLn(bool value) {
  printTrueFalse(value);
  Serial.println();
}
void printEnabledDisabled(bool value) {
  if(value) {
    Serial.print("enabled");
  } else {
    Serial.print("disabled");
  }
}
void printEnabledDisabledLn(bool value) {
  printEnabledDisabled(value);
  Serial.println();
}