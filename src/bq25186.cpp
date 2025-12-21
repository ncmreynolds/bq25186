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
	if(read_registers_()) {	//Read all registers at startup
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->println(F("BQ25186 library started"));
		}
		#endif
		return true;
	} else {
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->println(F("Unable to communicate with BQ25186"));
		}
		#endif
	}
	return false;
}
#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
void bq25186::debug(Stream &terminalStream) {
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
}
void bq25186::hexPrint(uint8_t value) {
	if(value < 0x10) {
		debug_uart_->print("0x0");
	} else {
		debug_uart_->print("0x");
	}
	debug_uart_->print(value,HEX);
}
void bq25186::hexPrintln(uint8_t value) {
	hexPrint(value);
	debug_uart_->println();
}
void bq25186::print_registers() {
	auto_refresh_all_registers_();
	if(debug_uart_ != nullptr) {
		for(uint8_t index = 0; index < bq25186_number_of_registers_; index++) {	//Iterate all the registers and print them
			debug_uart_->print(F("Register:"));
			hexPrint(index);
			debug_uart_->print(F(" value:"));
			hexPrintln(registers[index]);
		}
	}
}
#endif
bool bq25186::auto_refresh_all_registers_() {
	if(millis() - register_refresh_timer_ > register_refresh_rate_limit_) {
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		if(debug_uart_ != nullptr) {
			debug_uart_->println(F("Refreshing BQ25186 register values"));
		}
		#endif
		return read_registers_();
	}
	return false;
}
bool bq25186::read_registers_(uint8_t start, uint8_t length) {
	i2cPort_->beginTransmission(bq25186_i2c_address_);			//Start I2C transmission
	i2cPort_->write(start);										//Send the register to begin reading from
	if(i2cPort_->endTransmission() == 0)						//Check that it was sent
	{
		uint8_t bytesReceived = i2cPort_->requestFrom(bq25186_i2c_address_, bq25186_number_of_registers_);	//Request the registers
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
uint8_t bq25186::read_masked_value_from_register_(uint8_t index, uint8_t mask) {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return registers[index] & mask;
}
bool bq25186::write_register_(uint8_t index) {
	uint8_t data[2] = {index, registers[index]};	//Put the address and value together to send
	i2cPort_->beginTransmission(bq25186_i2c_address_);	//Start I2C transmission
	i2cPort_->write(data,2);							//Send the register and value
	if(i2cPort_->endTransmission() == 0) {				//Check that it was sent
		return true;
	}
	return false;
}
bool bq25186::write_masked_value_to_register_(uint8_t index, uint8_t mask, uint8_t value) {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	registers[index] = (registers[index] & mask) | value;
	return write_register_(index);
}
bool bq25186::ts_open() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[0], 7) == 1;
}
uint8_t bq25186::chg() {
	return read_masked_value_from_register_(0x00,0b01100000);
}
bool bq25186::ilim_active() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[0], 4) == 1;
}
bool bq25186::vdppm_active() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[0], 3) == 1;
}
bool bq25186::vindpm_active() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[0], 2) == 1;
}
bool bq25186::thermreg_active() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[0], 1) == 1;
}
bool bq25186::vin_pgood() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[0], 0) == 1;
}
bool bq25186::vin_ovp() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[1], 7) == 1;
}
bool bq25186::buvlo() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[1], 6) == 1;
}
uint8_t bq25186::ts() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return registers[1]&0b00011000;
}
bool bq25186::safety_tmr_fault_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[1], 2) == 1;
}
bool bq25186::wake1_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[1], 1) == 1;
}
bool bq25186::wake2_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[1], 0) == 1;
}
bool bq25186::ts_fault() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 7) == 1;
}
bool bq25186::ilim_active_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 6) == 1;
}
bool bq25186::vdppm_active_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 5) == 1;
}
bool bq25186::vindpm_active_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 4) == 1;
}
bool bq25186::thermreg_active_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 3) == 1;
}
bool bq25186::vin_ovp_fault_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 2) == 1;
}
bool bq25186::buvlo_fault_flag() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[1], 1) == 1;
}
bool bq25186::bat_ocp_fault() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[2], 0) == 1;
}
bool bq25186::get_pg_pin_mode() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[3], 7) == 1;
}
bool bq25186::set_pg_pin_mode(uint8_t value) {
	return write_masked_value_to_register_(0x03, 0b10000000, value);
}
float bq25186::get_vbatreg() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return 3.5 + ((registers[3]&0b01111111)*0.01);
}
bool bq25186::set_vbatreg(float voltage) {
	if(voltage >= 3.5 && voltage <= 4.65) {
		auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
		registers[3] = (registers[3]&0b10000000)|uint8_t((voltage-3.5)*100);
		return write_register_(3);
	}
	return false;
}
bool bq25186::get_chg_dis() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[4], 7) == 1;
}
bool bq25186::set_chg_dis(bool value) {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	if(value == true) {
		bitWrite(registers[4], 7, 1);
	} else {
		bitWrite(registers[4], 7, 0);
	}
	return write_register_(4);
}
uint16_t bq25186::get_ichg() {
	uint8_t registerValue = registers[4]&0b01111111;
	if(registerValue >= 31) {
		return 40+((registerValue-31)*10);
	} else {
		return (registerValue*5);
	}
}
bool bq25186::set_ichg(uint16_t value) {
	if(value >= 35 && value <=1000) {
		auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
		uint8_t registerValue = registers[4]&0b10000000;
		if(value >= 40) {
			registerValue = registerValue | ((value-40)/10);
		} else {
			registerValue = registerValue | ((value)/5);
		}
		return write_register_(4);
	}
	return false;
}
bool bq25186::get_en_fc_mode() {
	auto_refresh_all_registers_();			//Only refresh the registers if they have not been read recently
	return bitRead(registers[5], 7) == 1;
}
bool bq25186::set_en_fc_mode(bool value) {
	if(value == true) {
		bitWrite(registers[5], 7, 1);
	} else {
		bitWrite(registers[5], 7, 0);
	}
	return write_register_(5);
}
uint8_t bq25186::get_iprechg() {
	return read_masked_value_from_register_(5,0b01000000);
}
bool bq25186::set_iprechg(uint8_t value) {
	return write_masked_value_to_register_(5, 0b01000000, value);
}
uint8_t bq25186::get_iterm() {
	return read_masked_value_from_register_(5,0b00110000);
}
bool bq25186::set_iterm(uint8_t value) {
	return write_masked_value_to_register_(5, 0b00110000, value);
}
uint8_t bq25186::get_vindpm() {
	return read_masked_value_from_register_(5,0b00001100);
}
bool bq25186::set_vindpm(uint8_t value) {
	return write_masked_value_to_register_(5, 0b00001100, value);
}
uint8_t bq25186::get_therm_reg() {
	return read_masked_value_from_register_(5,0b00000011);
}
bool bq25186::set_therm_reg(uint8_t value) {
	return write_masked_value_to_register_(5, 0b00000011, value);
}
uint16_t bq25186::get_ibat_ocp() {
	switch(read_masked_value_from_register_(6,0b11000000)) {
	case BQ25186_IBAT_OCP_500MA:
		return 500;
	break;
	case BQ25186_IBAT_OCP_1000MA:
		return 1000;
	break;
	case BQ25186_IBAT_OCP_1500MA:
		return 1500;
	break;
	case BQ25186_IBAT_OCP_3000MA:
		return 3000;
	break;
	}
	return 0;
}
bool bq25186::set_ibat_ocp(uint8_t value) {
	return write_masked_value_to_register_(6, 0b11000000, value);
}
float bq25186::get_buvlo() {
	switch(read_masked_value_from_register_(6,0b00111000)) {
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
	return 0;
}
bool bq25186::set_buvlo(float value) {
	if(value > 2.8) {
		return write_masked_value_to_register_(6, 0b00111000, BQ25186_BUVLO_30A);
	} else if(value > 2.6) {
		return write_masked_value_to_register_(6, 0b00111000, BQ25186_BUVLO_28);
	} else if(value > 2.4) {
		return write_masked_value_to_register_(6, 0b00111000, BQ25186_BUVLO_26);
	} else if(value > 2.2) {
		return write_masked_value_to_register_(6, 0b00111000, BQ25186_BUVLO_24);
	} else if(value > 2.0) {
		return write_masked_value_to_register_(6, 0b00111000, BQ25186_BUVLO_22);		
	} else {
		return write_masked_value_to_register_(6, 0b00111000, BQ25186_BUVLO_20);
	}
}
uint8_t bq25186::get_chg_status_int_mask() {
	return read_masked_value_from_register_(6,0b00000100);
}
bool bq25186::set_chg_status_int_mask(uint8_t value) {
	return write_masked_value_to_register_(6, 0b00000100, value);
}
uint8_t bq25186::get_ilim_int_mask() {
	return read_masked_value_from_register_(6,0b00000010);
}
bool bq25186::set_ilim_int_mask(uint8_t value) {
	return write_masked_value_to_register_(6, 0b00000010, value);
}
uint8_t bq25186::get_vindpm_int_mask() {
	return read_masked_value_from_register_(6,0b00000001);
}
bool bq25186::set_vindpm_int_mask(uint8_t value) {
	return write_masked_value_to_register_(6, 0b00000001, value);
}
//Register 0x08
uint8_t bq25186::get_long_press_time() {
	switch(read_masked_value_from_register_(8,0b11000000)) {
	case BQ25186_MR_LPRESS_5S:
		return 5;
	break;
	case BQ25186_MR_LPRESS_10S:
		return 10;
	break;
	case BQ25186_MR_LPRESS_15S:
		return 15;
	break;
	case BQ25186_MR_LPRESS_20S:
		return 20;
	break;
	}
	return 0;
}
bool bq25186::set_long_press_time(uint8_t value) {							//Available values are 5/10/15/20s
	if(value >= 20) {
		return write_masked_value_to_register_(0x08, 0b11000000, BQ25186_MR_LPRESS_20S);
	} else if(value >= 15) {
		return write_masked_value_to_register_(0x08, 0b11000000, BQ25186_MR_LPRESS_15S);
	} else if(value >= 10) {
		return write_masked_value_to_register_(0x08, 0b11000000, BQ25186_MR_LPRESS_10S);
	} else {
		return write_masked_value_to_register_(0x08, 0b11000000, BQ25186_MR_LPRESS_5S);
	}
}
//Register 0x09
uint8_t bq25186::get_long_press_action() {
	return read_masked_value_from_register_(0x09,0b00011000);
}
bool bq25186::set_long_press_action(uint8_t value) {
	return write_masked_value_to_register_(0x09, 0b00011000, value);
}
//Register 0xa0
uint8_t bq25186::get_sys_regulation_voltgage() {
	return read_masked_value_from_register_(0xa0, 0b11100000);
}
bool bq25186::set_sys_regulation_voltgage(uint8_t value) {
	return write_masked_value_to_register_(0xa0, 0b11100000, value);
}
uint8_t bq25186::get_pg_pin_state() {
	return read_masked_value_from_register_(0xa0, 0b00010000);
}
bool bq25186::set_pg_pin_state(uint8_t value) {
	return write_masked_value_to_register_(0xa0, 0b00010000, value);
}
uint8_t bq25186::get_sys_mode() {
	return read_masked_value_from_register_(0xa0, 0b00001100);
}
bool bq25186::set_sys_mode(uint8_t value) {
	return write_masked_value_to_register_(0xa0, 0b00001100, value);
}
#endif
