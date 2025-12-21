/*
 *	An Arduino library to support the Texas Instruments BQ25186 (https://www.ti.com/product/BQ25186) "1A I²C-controlled linear battery charger with power path and solar input support"
 *
 *	https://github.com/ncmreynolds/bq25186
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/bq25186/blob/main/LICENSE for full license
 *
 */
 
#ifndef bq25186_h
#define bq25186_h
#include <Arduino.h>	//Include the Arduino library	
#include "Wire.h"	//Include the I²C library
#define BQ25186_INCLUDE_DEBUG_FUNCTIONS

#define BQ25186_ENABLED_BUT_NOT_CHARGING	0b00000000
#define BQ25186_CC_CHARGING					0b00100000
#define BQ25186_CV_CHARGING					0b01000000
#define BQ25186_CHARGING_DONE_OR_DISABLED	0b01100000
#define BQ25186_PG_PIN_MODE_PG				0b00000000
#define BQ25186_PG_PIN_MODE_GPO				0b10000000
#define BQ25186_PRECHARGE_1X				0b00000000
#define BQ25186_PRECHARGE_2X				0b01000000
#define BQ25186_ITERM_DISABLE				0b00000000
#define BQ25186_ITERM_5_PERCENT				0b00010000
#define BQ25186_ITERM_10_PERCENT			0b00100000
#define BQ25186_ITERM_20_PERCENT			0b00110000
#define BQ25186_VINDPM_VBAT_PLUS_300		0b00000000
#define BQ25186_VINDPM_4_5					0b00000100
#define BQ25186_VINDPM_4_7					0b00001000
#define BQ25186_VINDPM_DISABLED				0b00001100
#define BQ25186_THERM_REG_100C				0b00000000
#define BQ25186_THERM_REG_80C				0b00000001
#define BQ25186_THERM_REG_60C				0b00000010
#define BQ25186_THERM_REG_DISABLED			0b00000011
#define BQ25186_IBAT_OCP_500MA				0b00000000
#define BQ25186_IBAT_OCP_1000MA				0b01000000
#define BQ25186_IBAT_OCP_1500MA				0b10000000
#define BQ25186_IBAT_OCP_3000MA				0b11000000
#define BQ25186_BUVLO_30A					0b00000000
#define BQ25186_BUVLO_30B					0b00001000
#define BQ25186_BUVLO_30C					0b00010000
#define BQ25186_BUVLO_28					0b00011000
#define BQ25186_BUVLO_26					0b00100000
#define BQ25186_BUVLO_24					0b00101000
#define BQ25186_BUVLO_22					0b00110000
#define BQ25186_BUVLO_20					0b00111000
#define BQ25186_MR_LPRESS_5S				0b00000000
#define BQ25186_MR_LPRESS_10S				0b01000000
#define BQ25186_MR_LPRESS_15S				0b10000000
#define BQ25186_MR_LPRESS_20S				0b11000000
#define BQ25186_PB_LPRESS_ACTION_NOTHING	0b00000000
#define BQ25186_PB_LPRESS_ACTION_RESET		0b00001000
#define BQ25186_PB_LPRESS_ACTION_SHIP		0b00010000
#define BQ25186_PB_LPRESS_ACTION_SHUTDOWN	0b00011000
#define BQ25186_SYS_REG_CTRL_BATTERY_TRACK	0b00000000
#define BQ25186_SYS_REG_CTRL_4_4V			0b00100000
#define BQ25186_SYS_REG_CTRL_4_5V			0b01000000
#define BQ25186_SYS_REG_CTRL_4_6V			0b01100000
#define BQ25186_SYS_REG_CTRL_4_7V			0b10000000
#define BQ25186_SYS_REG_CTRL_4_8V			0b10100000
#define BQ25186_SYS_REG_CTRL_4_9V			0b11000000
#define BQ25186_SYS_REG_CTRL_PASS_THROUGH	0b11100000
#define BQ25186_SYS_MODE_VIN_OR_VBAT		0b00000000
#define BQ25186_SYS_MODE_VBAT_ONLY			0b00000100
#define BQ25186_PG_GPO_HIGH_IMPEDENCE		0b00000000
#define BQ25186_PG_GPO_LOW					0b00010000
#define BQ25186_SYS_MODE_FLOAT				0b00001000
#define BQ25186_SYS_MODE_PULLDOWN			0b00001100

class bq25186 {

	public:
		bq25186();															//Constructor function
		~bq25186();															//Destructor function
		bool begin(TwoWire &i2cPort = Wire);								//Start the bq25186
		//Readable flags/status, see device datasheet for explanation of the values
		//Register 0x01
		bool ts_open();
		uint8_t chg();
		bool ilim_active();
		bool vdppm_active();
		bool vindpm_active();
		bool thermreg_active();
		bool vin_pgood();
		bool vin_ovp();
		bool buvlo();
		uint8_t ts();
		bool safety_tmr_fault_flag();
		bool wake1_flag();
		bool wake2_flag();
		//Register 0x03
		bool ts_fault();
		bool ilim_active_flag();
		bool vdppm_active_flag();
		bool vindpm_active_flag();
		bool thermreg_active_flag();
		bool vin_ovp_fault_flag();
		bool buvlo_fault_flag();
		bool bat_ocp_fault();
		//Register 0x03
		bool get_pg_pin_mode();
		bool set_pg_pin_mode(uint8_t value);
		float get_vbatreg();
		bool set_vbatreg(float voltage);
		//Register 0x04
		bool get_chg_dis();
		bool set_chg_dis(bool value);
		uint16_t get_ichg();
		bool set_ichg(uint16_t value);
		//Register 0x05
		bool get_en_fc_mode();
		bool set_en_fc_mode(bool value);
		uint8_t get_iprechg();
		bool set_iprechg(uint8_t value);
		uint8_t get_iterm();
		bool set_iterm(uint8_t value);
		uint8_t get_vindpm();
		bool set_vindpm(uint8_t value);
		uint8_t get_therm_reg();
		bool set_therm_reg(uint8_t value);
		//Register 0x06
		uint16_t get_ibat_ocp();
		bool set_ibat_ocp(uint8_t value);
		float get_buvlo();
		bool set_buvlo(float value);
		uint8_t get_chg_status_int_mask();
		bool set_chg_status_int_mask(uint8_t value);
		uint8_t get_ilim_int_mask();
		bool set_ilim_int_mask(uint8_t value);
		uint8_t get_vindpm_int_mask();
		bool set_vindpm_int_mask(uint8_t value);
		//Register 0x07
		//Register 0x08
		uint8_t get_long_press_time();
		bool set_long_press_time(uint8_t value);							//Available values are 5/10/15/20s
		//Register 0x09
		uint8_t get_long_press_action();
		bool set_long_press_action(uint8_t value);							//Available values are 5/10/15/20s
		//Register 0xa0
		uint8_t get_sys_regulation_voltgage();
		bool set_sys_regulation_voltgage(uint8_t value);
		uint8_t get_pg_pin_state();
		bool set_pg_pin_state(uint8_t value);
		uint8_t get_sys_mode();
		bool set_sys_mode(uint8_t value);
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		void debug(Stream &);												//Start debugging on a stream
		void print_registers();												//Print all the registers to the debug Stream
		#endif
	protected:
	private:
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		Stream *debug_uart_ = nullptr;										//The stream used for the debugging
		void hexPrint(uint8_t value);										//Oh why am I doing this?
		void hexPrintln(uint8_t value);										//That's because I can't rely on the existence of printf in Arduino!
		#endif
		TwoWire *i2cPort_ = nullptr;										//Pointer to I²C instance used by library
		const uint8_t bq25186_i2c_address_ = 0x6a;							//This can't be changed
		static const uint8_t bq25186_number_of_registers_ = 0x0d;
		uint8_t registers[bq25186_number_of_registers_];					//Storage for the BQ2518 registers
		uint32_t register_refresh_timer_ = 0;								//Rate limit register reads
		uint32_t register_refresh_rate_limit_ = 100;						//Rate limit defaults to once every 100ms
		
		bool read_registers_(uint8_t start = 0x00, uint8_t length = 0x0d);	//Read registers, normally automatic before any other status command
		uint8_t read_masked_value_from_register_(uint8_t index,				//Read a bitmasked value from a register
			uint8_t mask);
		bool write_masked_value_to_register_(uint8_t index, uint8_t mask,	//Write a bitmasked value to a register
			uint8_t value);
		bool write_register_(uint8_t register);								//Write a specific register
		bool auto_refresh_all_registers_();									//Automatic refresh of all registers before any action
};
#endif
