/*
 *	An Arduino library to support the Texas Instruments BQ25186 (https://www.ti.com/product/BQ25186) "1A I²C-controlled linear battery charger with power path and solar input support"
 *
 *	https://github.com/ncmreynolds/bq25186
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/bq25186/blob/main/LICENSE for full license
 *
 */

#ifndef bq25186_cpp
#define bq25186_cpp
#include "bq25186.h"


bq25186::bq25186()	//Constructor function
{
}

bq25186::~bq25186()	//Destructor function
{
}

bool bq25186::begin(TwoWire &wirePort) {
	i2cPort_ = &wirePort;			//Set the wire instance used for the charger
	bq25186_communicating_ok_ = read_registers_();
	if(bq25186_communicating_ok_) {	//Read all registers at startup
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->println(F("BQ25186 library started"));
		}
		#endif
	} else {
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->println(F("Unable to communicate with BQ25186"));
		}
		#endif
	}
	return bq25186_communicating_ok_;
}
#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
void bq25186::debug(Stream &terminalStream) {
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
}
void bq25186::printHex(uint8_t value) {
	if(value < 0x10) {
		debug_uart_->print("0x0");
	} else {
		debug_uart_->print("0x");
	}
	debug_uart_->print(value,HEX);
}
void bq25186::printHexln(uint8_t value) {
	printHex(value);
	debug_uart_->println();
}
void bq25186::printBinary(uint8_t value) {
	for(uint8_t index = 0; index < 8;index++){
		if((0x80 >> index) & value) {
			debug_uart_->print('1');
		} else {
			debug_uart_->print('0');
		}
	}
}
void bq25186::printBinaryLn(uint8_t value) {
	printBinary(value);
	debug_uart_->println();
}
void bq25186::print_registers() {
	auto_refresh_all_registers_();
	if(debug_uart_ != nullptr) {
		for(uint8_t index = 0; index < bq25186_number_of_registers_; index++) {	//Iterate all the registers and print them
			debug_uart_->print(F("Register:"));
			printHex(index);
			debug_uart_->print(F(" value:"));
			printHex(registers[index]);
			debug_uart_->print('/');
			printBinaryLn(registers[index]);
		}
	}
}
#endif
bool bq25186::auto_refresh_all_registers_() {
	if(millis() - register_refresh_timer_ > register_refresh_rate_limit_) {
		register_refresh_timer_ = millis();
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->print(F("Refreshing BQ25186 register values "));
		}
		#endif
		bq25186_communicating_ok_ = read_registers_();
	}
	return bq25186_communicating_ok_;
}
bool bq25186::read_registers_(uint8_t start, uint8_t length, bool stop) {
	i2cPort_->beginTransmission(bq25186_i2c_address_);			//Start I2C transmission
	i2cPort_->write(start);										//Send the register to begin reading from
	if(i2cPort_->endTransmission() == 0)						//Check that it was sent
	{
		uint8_t bytesReceived = i2cPort_->requestFrom(bq25186_i2c_address_, bq25186_number_of_registers_, stop);	//Request the registers and optionally send a 'stop'
		if(bytesReceived == length) {
			while(i2cPort_->available() && bytesReceived > 0) {
				registers[length-bytesReceived] = i2cPort_->read();		//Read the current byte
				bytesReceived--;
			}
			if(bytesReceived == 0) {
				#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
				if(debug_uart_ != nullptr) {
					debug_uart_->println(F("Succesfully read BQ25186 registers"));
				}
				#endif
				return true;
			} else {
				#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
				if(debug_uart_ != nullptr) {
					debug_uart_->println(F("Incomplete read of BQ25186 registers"));
				}
				#endif
				return false;
			}
		} else {
			#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
			if(debug_uart_ != nullptr) {
				debug_uart_->println(F("Unable to read BQ25186 registers"));
			}
			#endif
		}
	}
	return false;
}
uint8_t bq25186::read_bitmasked_value_from_register_(uint8_t index, uint8_t mask) {
	if(auto_refresh_all_registers_()) {			//Only refreshes the registers if they have not been read recently
		return registers[index] & mask;
	} else {
		return BQ25186_I2C_ERROR;
	}
}
bool bq25186::write_register_(uint8_t registerIndex, uint8_t registerValue, bool stop) {
	i2cPort_->beginTransmission(bq25186_i2c_address_);	//Start I2C transmission
	uint8_t i2cData[2] = {registerIndex, registerValue};//Put the address and value together to send
	i2cPort_->write(i2cData,2);							//Send the register and value
	uint8_t i2cError = i2cPort_->endTransmission(stop);	//End the transmission, with a stop
	if(i2cError == 0) {									//Check that it was sent
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
			if(debug_uart_ != nullptr) {
				debug_uart_->println(F("Register write succeeded"));
			}
		#endif
		return true;
	}
	#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->print(F("Register write failed, error:"));
			switch (i2cError) {
				  case 1:
					debug_uart_->println(F("data too long to fit in transmit buffer"));
					break;
				  case 2:
					debug_uart_->println(F("received NACK on transmit of address"));
					break;
				  case 3:
					debug_uart_->println(F("received NACK on transmit of data"));
					break;
				  case 4:
					debug_uart_->println(F("other error"));
					break;
				  case 5:
					debug_uart_->println(F("timeout"));
					break;
				  default:
					debug_uart_->println(F("unknown"));
					break;
			}
		}
	#endif
	return false;
}
bool bq25186::write_bitmasked_value_to_register_(uint8_t index, uint8_t mask, uint8_t value) {
	#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
	if(debug_uart_ != nullptr) {
		debug_uart_->print(F("Writing bit masked value to register:"));
		printHex(index);
		debug_uart_->print(F(" mask:"));
		printBinary(mask);
		debug_uart_->print(F(" value:"));
		printBinary(value);
		debug_uart_->println();
	}
	#endif
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
	if(debug_uart_ != nullptr) {
		debug_uart_->print(F("Current value:"));
		printHex(registers[index]);
		debug_uart_->print('/');
		printBinary(registers[index]);
	}
	#endif
	uint8_t newValue = (registers[index] & (mask ^ 0xff)) | (value & mask);
	#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
	if(debug_uart_ != nullptr) {
		debug_uart_->print(F(" new value:"));
		printHex(newValue);
		debug_uart_->print('/');
		printBinaryLn(newValue);
	}
	#endif
	if(write_register_(index, newValue)) {
		registers[index] = newValue;
		return true;
	}
	return false;
}
//Register 0x00
uint8_t bq25186::ts_open_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_7);
}
/*
Get charging status, returns...

	BQ25186_ENABLED_BUT_NOT_CHARGING
	BQ25186_CC_CHARGING
	BQ25186_CV_CHARGING
	BQ25186_CHARGING_DONE_OR_DISABLED
	BQ25186_I2C_ERROR
*/
uint8_t bq25186::chg_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_6_5);
}
uint8_t bq25186::ilim_active_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_4);
}
uint8_t bq25186::vdppm_active_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_3);
}
uint8_t bq25186::vindpm_active_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_2);
}
uint8_t bq25186::thermreg_active_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_1);
}
uint8_t bq25186::vin_pgood_stat() {
	return read_bitmasked_value_from_register_(0x00,BQ25186_I2C_BITMASK_0);
}
//Register 0x01
uint8_t bq25186::vin_ovp_stat() {
	return read_bitmasked_value_from_register_(0x01,BQ25186_I2C_BITMASK_7);
}
uint8_t bq25186::buvlo_stat() {
	return read_bitmasked_value_from_register_(0x01,BQ25186_I2C_BITMASK_6);
}
uint8_t bq25186::ts_stat() {
	return read_bitmasked_value_from_register_(0x01,BQ25186_I2C_BITMASK_4_3);
}
uint8_t bq25186::safety_tmr_fault_flag() {
	return read_bitmasked_value_from_register_(0x01,BQ25186_I2C_BITMASK_2);
}
uint8_t bq25186::wake1_flag() {
	return read_bitmasked_value_from_register_(0x01,BQ25186_I2C_BITMASK_1);
}
uint8_t bq25186::wake2_flag() {
	return read_bitmasked_value_from_register_(0x01,BQ25186_I2C_BITMASK_0);
}
//Register 0x02
uint8_t bq25186::ts_fault() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_7);
}
uint8_t bq25186::ilim_active_flag() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_6);
}
uint8_t bq25186::vdppm_active_flag() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_5);
}
uint8_t bq25186::vindpm_active_flag() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_4);
}
uint8_t bq25186::thermreg_active_flag() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_3);
}
uint8_t bq25186::vin_ovp_fault_flag() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_2);
}
uint8_t bq25186::buvlo_fault_flag() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_1);
}
uint8_t bq25186::bat_ocp_fault() {
	return read_bitmasked_value_from_register_(0x02,BQ25186_I2C_BITMASK_0);
}
//Register 0x03
uint8_t bq25186::get_pg_pin_mode() {
	return read_bitmasked_value_from_register_(0x03,BQ25186_I2C_BITMASK_7);
}
bool bq25186::set_pg_pin_mode(uint8_t value) {
	return write_bitmasked_value_to_register_(0x03, BQ25186_I2C_BITMASK_7, value);
}
float bq25186::get_vbatreg() {
	uint8_t vbatreg = read_bitmasked_value_from_register_(0x03,BQ25186_I2C_BITMASK_6_0);
	if(vbatreg != BQ25186_I2C_ERROR) {
		return 3.5 + (float(vbatreg)*0.01);
	} else {
		return 0;
	}
}
bool bq25186::set_vbatreg(float voltage) {
	if(voltage >= 3.5 && voltage <= 4.65) {
		return write_bitmasked_value_to_register_(0x03, BQ25186_I2C_BITMASK_6_0, uint8_t((voltage-3.5)*100));
	}
	return false;
}
//Register 0x04
uint8_t bq25186::get_chg_dis() {
	return read_bitmasked_value_from_register_(0x04,BQ25186_I2C_BITMASK_7);
}
bool bq25186::set_chg_dis(uint8_t value) {
	return write_bitmasked_value_to_register_(0x04, BQ25186_I2C_BITMASK_7, value);
}
uint16_t bq25186::get_ichg() {
	uint8_t maskedRegisterValue = read_bitmasked_value_from_register_(0x04,BQ25186_I2C_BITMASK_6_0);
	if(maskedRegisterValue == BQ25186_I2C_ERROR) {
		return 0;
	} else if(maskedRegisterValue > 31) {
		return 40+((maskedRegisterValue-31)*10);
	} else {
		return (maskedRegisterValue+5);
	}
}
bool bq25186::set_ichg(uint16_t value) {
	if(value <= 1000) {
		uint8_t maskedRegisterValue = 0;
		if(value > 35) {
			maskedRegisterValue = 32+((value-50)/10);
		} else {
			maskedRegisterValue = value-5;
		}
		return write_bitmasked_value_to_register_(0x04, BQ25186_I2C_BITMASK_6_0, maskedRegisterValue);
	}
	return false;
}
//Register 0x05
uint8_t bq25186::get_en_fc_mode() {
	return read_bitmasked_value_from_register_(0x05,BQ25186_I2C_BITMASK_7);
}
bool bq25186::set_en_fc_mode(uint8_t value) {
	return write_bitmasked_value_to_register_(0x05, BQ25186_I2C_BITMASK_7, value);
}
uint8_t bq25186::get_iprechg() {
	return read_bitmasked_value_from_register_(0x05,BQ25186_I2C_BITMASK_6);
}
bool bq25186::set_iprechg(uint8_t value) {
	return write_bitmasked_value_to_register_(0x05, BQ25186_I2C_BITMASK_6, value);
}
uint8_t bq25186::get_iterm() {
	return read_bitmasked_value_from_register_(0x05,BQ25186_I2C_BITMASK_5_4);
}
bool bq25186::set_iterm(uint8_t value) {
	return write_bitmasked_value_to_register_(0x05, BQ25186_I2C_BITMASK_5_4, value);
}
uint8_t bq25186::get_vindpm() {
	return read_bitmasked_value_from_register_(0x05,BQ25186_I2C_BITMASK_3_2);
}
bool bq25186::set_vindpm(uint8_t value) {
	return write_bitmasked_value_to_register_(0x05, BQ25186_I2C_BITMASK_3_2, value);
}
uint8_t bq25186::get_therm_reg() {
	return read_bitmasked_value_from_register_(0x05,BQ25186_I2C_BITMASK_1_0);
}
bool bq25186::set_therm_reg(uint8_t value) {
	return write_bitmasked_value_to_register_(0x05, BQ25186_I2C_BITMASK_1_0, value);
}
//Register 0x06
uint8_t bq25186::get_ibat_ocp() {
	return read_bitmasked_value_from_register_(0x06,BQ25186_I2C_BITMASK_7_6);
}
bool bq25186::set_ibat_ocp(uint8_t value) {
	return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_7_6, value);
}
float bq25186::get_buvlo() {
	uint8_t buvlo = read_bitmasked_value_from_register_(0x06,BQ25186_I2C_BITMASK_5_3);
	if(buvlo != BQ25186_I2C_ERROR) {
		switch(buvlo) {
		case BQ25186_BUVLO_30A:
			return 3.0;
		break;
		case BQ25186_BUVLO_30B:
			return 3.0;
		break;
		case BQ25186_BUVLO_30C:
			return 3.0;
		break;
		case BQ25186_BUVLO_28:
			return 2.8;
		break;
		case BQ25186_BUVLO_26:
			return 2.6;
		break;
		case BQ25186_BUVLO_24:
			return 2.4;
		break;
		case BQ25186_BUVLO_22:
			return 2.2;
		break;
		case BQ25186_BUVLO_20:
			return 2.0;
		break;
		}
	}
	return 0;
}
bool bq25186::set_buvlo(float value) {
	if(value > 2.8) {
		return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_5_3, BQ25186_BUVLO_30A);
	} else if(value > 2.6) {
		return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_5_3, BQ25186_BUVLO_28);
	} else if(value > 2.4) {
		return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_5_3, BQ25186_BUVLO_26);
	} else if(value > 2.2) {
		return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_5_3, BQ25186_BUVLO_24);
	} else if(value > 2.0) {
		return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_5_3, BQ25186_BUVLO_22);		
	} else {
		return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_5_3, BQ25186_BUVLO_20);
	}
}
uint8_t bq25186::get_chg_status_int_mask() {
	return read_bitmasked_value_from_register_(0x06,BQ25186_I2C_BITMASK_2);
}
bool bq25186::set_chg_status_int_mask(uint8_t value) {
	return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_2, value);
}
uint8_t bq25186::get_ilim_int_mask() {
	return read_bitmasked_value_from_register_(0x06,BQ25186_I2C_BITMASK_1);
}
bool bq25186::set_ilim_int_mask(uint8_t value) {
	return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_1, value);
}
uint8_t bq25186::get_vindpm_int_mask() {
	return read_bitmasked_value_from_register_(0x06,BQ25186_I2C_BITMASK_0);
}
bool bq25186::set_vindpm_int_mask(uint8_t value) {
	return write_bitmasked_value_to_register_(0x06, BQ25186_I2C_BITMASK_0, value);
}
//Register 0x07
//ToDo

//Register 0x08
uint8_t bq25186::get_long_press_time() {
	return read_bitmasked_value_from_register_(0x08,BQ25186_I2C_BITMASK_7_6);
}
bool bq25186::set_long_press_time(uint8_t value) {
	return write_bitmasked_value_to_register_(0x08, BQ25186_I2C_BITMASK_7_6, value);
}
uint8_t bq25186::get_mr_reset_vin() {
	return read_bitmasked_value_from_register_(0x08,BQ25186_I2C_BITMASK_5);
}
bool bq25186::set_mr_reset_vin(uint8_t value) {
	return write_bitmasked_value_to_register_(0x08, BQ25186_I2C_BITMASK_5, value);
}
uint8_t bq25186::get_autowake() {
	return read_bitmasked_value_from_register_(0x08,BQ25186_I2C_BITMASK_4_3);
}
bool bq25186::set_autowake(uint8_t value) {
	return write_bitmasked_value_to_register_(0x08, BQ25186_I2C_BITMASK_4_3, value);
}
uint8_t bq25186::get_ilim() {
	return read_bitmasked_value_from_register_(0x08,BQ25186_I2C_BITMASK_4_3);
}
bool bq25186::set_ilim(uint8_t value) {
	return write_bitmasked_value_to_register_(0x08, BQ25186_I2C_BITMASK_4_3, value);
}
//Register 0x09
uint8_t bq25186::get_reg_rst() {
	return read_bitmasked_value_from_register_(0x09,BQ25186_I2C_BITMASK_7);
}
bool bq25186::set_reg_rst(uint8_t value) {
	return write_bitmasked_value_to_register_(0x09, BQ25186_I2C_BITMASK_7, value);
}
uint8_t bq25186::get_reset_ship() {
	return read_bitmasked_value_from_register_(0x09,BQ25186_I2C_BITMASK_6_5);
}
bool bq25186::set_reset_ship(uint8_t value) {
	return write_bitmasked_value_to_register_(0x09, BQ25186_I2C_BITMASK_6_5, value);
}
uint8_t bq25186::get_long_press_action() {
	return read_bitmasked_value_from_register_(0x09,BQ25186_I2C_BITMASK_4_3);
}
bool bq25186::set_long_press_action(uint8_t value) {
	return write_bitmasked_value_to_register_(0x09, BQ25186_I2C_BITMASK_4_3, value);
}
uint8_t bq25186::get_wake1_tmr() {
	return read_bitmasked_value_from_register_(0x09,BQ25186_I2C_BITMASK_2);
}
bool bq25186::set_wake1_tmr(uint8_t value) {
	return write_bitmasked_value_to_register_(0x09, BQ25186_I2C_BITMASK_2, value);
}
uint8_t bq25186::get_wake2_tmr() {
	return read_bitmasked_value_from_register_(0x09,BQ25186_I2C_BITMASK_1);
}
bool bq25186::set_wake2_tmr(uint8_t value) {
	return write_bitmasked_value_to_register_(0x09, BQ25186_I2C_BITMASK_1, value);
}
uint8_t bq25186::get_en_push() {
	return read_bitmasked_value_from_register_(0x09,BQ25186_I2C_BITMASK_0);
}
bool bq25186::set_en_push(uint8_t value) {
	return write_bitmasked_value_to_register_(0x09, BQ25186_I2C_BITMASK_0, value);
}

//Register 0x0a
uint8_t bq25186::get_sys_regulation_voltage() {
	return read_bitmasked_value_from_register_(0x0a, 0b11100000);
}
bool bq25186::set_sys_regulation_voltage(uint8_t value) {
	return write_bitmasked_value_to_register_(0x0a, 0b11100000, value);
}
uint8_t bq25186::get_pg_pin_state() {
	return read_bitmasked_value_from_register_(0x0a, 0b00010000);
}
bool bq25186::set_pg_pin_state(uint8_t value) {
	return write_bitmasked_value_to_register_(0x0a, 0b00010000, value);
}
uint8_t bq25186::get_sys_mode() {
	return read_bitmasked_value_from_register_(0x0a, 0b00001100);
}
bool bq25186::set_sys_mode(uint8_t value) {
	return write_bitmasked_value_to_register_(0x0a, 0b00001100, value);
}
uint8_t bq25186::get_i2c_watchdog_mode() {
	return read_bitmasked_value_from_register_(0x0a, 0b00000010);
}
bool bq25186::set_i2c_watchdog_mode(uint8_t value) {
	return write_bitmasked_value_to_register_(0x0a, 0b00000010, value);
}
#endif
