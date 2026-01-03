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
#include "Wire.h"		//Include the I²C library

#define BQ25186_INCLUDE_DEBUG_FUNCTIONS

//These defines are an attempt to make all the bitmask work legible without introducing tons of abstraction

#define BQ25186_I2C_BITMASK_7				0b10000000
#define BQ25186_I2C_BITMASK_7_6				0b11000000
#define BQ25186_I2C_BITMASK_75				0b10100000
#define BQ25186_I2C_BITMASK_7_5				0b11100000
#define BQ25186_I2C_BITMASK_6				0b01000000
#define BQ25186_I2C_BITMASK_6_5				0b01100000
#define BQ25186_I2C_BITMASK_6_0				0b01111111
#define BQ25186_I2C_BITMASK_5				0b00100000
#define BQ25186_I2C_BITMASK_5_4				0b00110000
#define BQ25186_I2C_BITMASK_5_3				0b00111000
#define BQ25186_I2C_BITMASK_4				0b00010000
#define BQ25186_I2C_BITMASK_4_3				0b00011000
#define BQ25186_I2C_BITMASK_3				0b00001000
#define BQ25186_I2C_BITMASK_3_2				0b00001100
#define BQ25186_I2C_BITMASK_2				0b00000100
#define BQ25186_I2C_BITMASK_21				0b00000110
#define BQ25186_I2C_BITMASK_20				0b00000101
#define BQ25186_I2C_BITMASK_1				0b00000010
#define BQ25186_I2C_BITMASK_1_0				0b00000011
#define BQ25186_I2C_BITMASK_0				0b00000001
#define BQ25186_I2C_BITMASK_NONE			0b00000000
#define BQ25186_I2C_ERROR				0b11111111

//Register 0x00

#define BQ25186_TSMR_NOT_OPEN				BQ25186_I2C_BITMASK_NONE
#define BQ25186_TSMR_OPEN					BQ25186_I2C_BITMASK_7

#define BQ25186_ENABLED_BUT_NOT_CHARGING	BQ25186_I2C_BITMASK_NONE
#define BQ25186_CC_CHARGING					BQ25186_I2C_BITMASK_5
#define BQ25186_CV_CHARGING					BQ25186_I2C_BITMASK_6
#define BQ25186_CHARGING_DONE_OR_DISABLED	BQ25186_I2C_BITMASK_6_5

#define BQ25186_ILIM_NOT_ACTIVE				BQ25186_I2C_BITMASK_NONE
#define BQ25186_ILIM_ACTIVE					BQ25186_I2C_BITMASK_4

#define BQ25186_VDPPM_NOT_ACTIVE			BQ25186_I2C_BITMASK_NONE
#define BQ25186_VDPPM_ACTIVE				BQ25186_I2C_BITMASK_3

#define BQ25186_VINDPM_NOT_ACTIVE			BQ25186_I2C_BITMASK_NONE
#define BQ25186_VINDPM_ACTIVE				BQ25186_I2C_BITMASK_2

#define BQ25186_THERMREG_NOT_ACTIVE			BQ25186_I2C_BITMASK_NONE
#define BQ25186_THERMREG_ACTIVE				BQ25186_I2C_BITMASK_1

#define BQ25186_POWER_NOT_GOOD				BQ25186_I2C_BITMASK_NONE
#define BQ25186_POWER_GOOD					BQ25186_I2C_BITMASK_0

//Register 0x01

#define BQ25186_VIN_OVP_NOT_ACTIVE			BQ25186_I2C_BITMASK_NONE
#define BQ25186_VIN_OVP_ACTIVE				BQ25186_I2C_BITMASK_7

#define BQ25186_BUVLO_NOT_ACTIVE			BQ25186_I2C_BITMASK_NONE
#define BQ25186_BUVLO_ACTIVE				BQ25186_I2C_BITMASK_6

#define BQ25186_TS_NORMAL					BQ25186_I2C_BITMASK_NONE
#define BQ25186_TS_TOO_HOT_OR_COLD			BQ25186_I2C_BITMASK_3
#define BQ25186_TS_COOL						BQ25186_I2C_BITMASK_4
#define BQ25186_TS_WARM						BQ25186_I2C_BITMASK_4_3

#define BQ25186_SAFETY_TMR_NOT_ACTIVE		BQ25186_I2C_BITMASK_NONE
#define BQ25186_SAFETY_TMR_ACTIVE			BQ25186_I2C_BITMASK_2

#define BQ25186_WAKE1_FLAG_NOT_ACTIVE		BQ25186_I2C_BITMASK_NONE
#define BQ25186_WAKE1_FLAG_ACTIVE			BQ25186_I2C_BITMASK_1

#define BQ25186_WAKE2_FLAG_NOT_ACTIVE		BQ25186_I2C_BITMASK_NONE
#define BQ25186_WAKE2_FLAG_ACTIVE			BQ25186_I2C_BITMASK_0

//Register 0x02

#define BQ25186_TS_FAULT_NOT_DETECTED		BQ25186_I2C_BITMASK_NONE
#define BQ25186_TS_FAULT_DETECTED			BQ25186_I2C_BITMASK_7

#define BQ25186_ILIM_FAULT_NOT_DETECTED		BQ25186_I2C_BITMASK_NONE
#define BQ25186_ILIM_FAULT_DETECTED			BQ25186_I2C_BITMASK_6

#define BQ25186_VDPPM_FAULT_NOT_DETECTED	BQ25186_I2C_BITMASK_NONE
#define BQ25186_VDPPM_FAULT_DETECTED		BQ25186_I2C_BITMASK_5

#define BQ25186_VINDPM_FAULT_NOT_DETECTED	BQ25186_I2C_BITMASK_NONE
#define BQ25186_VINDPM_FAULT_DETECTED		BQ25186_I2C_BITMASK_4

#define BQ25186_THERMREG_FAULT_NOT_DETECTED	BQ25186_I2C_BITMASK_NONE
#define BQ25186_THERMREG_FAULT_DETECTED		BQ25186_I2C_BITMASK_3

#define BQ25186_VIN_OVP_FAULT_NOT_DETECTED	BQ25186_I2C_BITMASK_NONE
#define BQ25186_VIN_OVP_FAULT_DETECTED		BQ25186_I2C_BITMASK_2

#define BQ25186_BUVLO_FAULT_NOT_DETECTED	BQ25186_I2C_BITMASK_NONE
#define BQ25186_BUVLO_FAULT_DETECTED		BQ25186_I2C_BITMASK_1

#define BQ25186_BAT_OCP_FAULT_NOT_DETECTED	BQ25186_I2C_BITMASK_NONE
#define BQ25186_BAT_OCP_FAULT_DETECTED		BQ25186_I2C_BITMASK_0

//Register 0x03
#define BQ25186_PG_PIN_MODE_PG				BQ25186_I2C_BITMASK_NONE
#define BQ25186_PG_PIN_MODE_GPO				BQ25186_I2C_BITMASK_7

//Register 0x04
#define BQ25186_CHG_ENABLED					BQ25186_I2C_BITMASK_NONE
#define BQ25186_CHG_DISABLED				BQ25186_I2C_BITMASK_7

//Register 0x05
#define BQ25186_FLASH_CHG_DISABLED			BQ25186_I2C_BITMASK_NONE
#define BQ25186_FLASH_CHG_ENABLED			BQ25186_I2C_BITMASK_7

#define BQ25186_IPRECHARGE_2X				BQ25186_I2C_BITMASK_NONE
#define BQ25186_IPRECHARGE_1X				BQ25186_I2C_BITMASK_6

#define BQ25186_ITERM_DISABLE				BQ25186_I2C_BITMASK_NONE
#define BQ25186_ITERM_5_PERCENT				BQ25186_I2C_BITMASK_4
#define BQ25186_ITERM_10_PERCENT			BQ25186_I2C_BITMASK_5
#define BQ25186_ITERM_20_PERCENT			BQ25186_I2C_BITMASK_5_4

#define BQ25186_VINDPM_VBAT_PLUS_300		BQ25186_I2C_BITMASK_NONE
#define BQ25186_VINDPM_4_5					BQ25186_I2C_BITMASK_2
#define BQ25186_VINDPM_4_7					BQ25186_I2C_BITMASK_3
#define BQ25186_VINDPM_DISABLED				BQ25186_I2C_BITMASK_3_2

#define BQ25186_THERM_REG_100C				BQ25186_I2C_BITMASK_NONE
#define BQ25186_THERM_REG_80C				BQ25186_I2C_BITMASK_0
#define BQ25186_THERM_REG_60C				BQ25186_I2C_BITMASK_1
#define BQ25186_THERM_REG_DISABLED			BQ25186_I2C_BITMASK_1_0

//Register 0x06

#define BQ25186_IBAT_OCP_500MA				BQ25186_I2C_BITMASK_NONE
#define BQ25186_IBAT_OCP_1000MA				BQ25186_I2C_BITMASK_6
#define BQ25186_IBAT_OCP_1500MA				BQ25186_I2C_BITMASK_7
#define BQ25186_IBAT_OCP_3000MA				BQ25186_I2C_BITMASK_7_6

#define BQ25186_BUVLO_30A					BQ25186_I2C_BITMASK_NONE
#define BQ25186_BUVLO_30B					BQ25186_I2C_BITMASK_3
#define BQ25186_BUVLO_30C					BQ25186_I2C_BITMASK_4
#define BQ25186_BUVLO_28					BQ25186_I2C_BITMASK_4_3
#define BQ25186_BUVLO_26					BQ25186_I2C_BITMASK_5
#define BQ25186_BUVLO_24					0b00101000
#define BQ25186_BUVLO_22					BQ25186_I2C_BITMASK_5_4
#define BQ25186_BUVLO_20					0b00111000

#define BQ25186_CHG_STATUS_INT_ENABLED		BQ25186_I2C_BITMASK_NONE
#define BQ25186_CHG_STATUS_INT_DISABLED		BQ25186_I2C_BITMASK_2

#define BQ25186_ILIM_INT_ENABLED			BQ25186_I2C_BITMASK_NONE
#define BQ25186_ILIM_INT_DISABLED			BQ25186_I2C_BITMASK_1

#define BQ25186_VINDPM_INT_ENABLED			BQ25186_I2C_BITMASK_NONE
#define BQ25186_VINDPM_INT_DISABLED			BQ25186_I2C_BITMASK_0

//Register 0x06
//ToDo

//Register 0x08

#define BQ25186_MR_LPRESS_5S				BQ25186_I2C_BITMASK_NONE
#define BQ25186_MR_LPRESS_10S				BQ25186_I2C_BITMASK_6
#define BQ25186_MR_LPRESS_15S				BQ25186_I2C_BITMASK_7
#define BQ25186_MR_LPRESS_20S				BQ25186_I2C_BITMASK_7_6

#define BQ25186_MR_RESET_NOT_REQ_VIN		BQ25186_I2C_BITMASK_NONE
#define BQ25186_MR_RESET_REQ_VIN			BQ25186_I2C_BITMASK_5

#define BQ25186_AUTOWAKE_0_5_S				BQ25186_I2C_BITMASK_NONE
#define BQ25186_AUTOWAKE_1_S				BQ25186_I2C_BITMASK_3
#define BQ25186_AUTOWAKE_2_S				BQ25186_I2C_BITMASK_4
#define BQ25186_AUTOWAKE_4_S				BQ25186_I2C_BITMASK_4_3

#define BQ25186_ILIM_50_MA					BQ25186_I2C_BITMASK_NONE
#define BQ25186_ILIM_100_MA					BQ25186_I2C_BITMASK_0
#define BQ25186_ILIM_200_MA					BQ25186_I2C_BITMASK_1
#define BQ25186_ILIM_300_MA					BQ25186_I2C_BITMASK_1_0
#define BQ25186_ILIM_400_MA					BQ25186_I2C_BITMASK_2
#define BQ25186_ILIM_500_MA					BQ25186_I2C_BITMASK_20
#define BQ25186_ILIM_665_MA					BQ25186_I2C_BITMASK_2_1
#define BQ25186_ILIM_1050_MA				BQ25186_I2C_BITMASK_2_0

//Register 0x09

#define BQ25186_NO_SOFTWARE_RESET			BQ25186_I2C_BITMASK_NONE
#define BQ25186_SOFTWARE_RESET				BQ25186_I2C_BITMASK_7

#define BQ25186_NO_HARDWARE_RESET			BQ25186_I2C_BITMASK_NONE
#define BQ25186_SHUTDOWN_MODE				BQ25186_I2C_BITMASK_5
#define BQ25186_SHIP_MODE					BQ25186_I2C_BITMASK_6
#define BQ25186_HARDWARE_RESET				BQ25186_I2C_BITMASK_6_5

#define BQ25186_PB_LPRESS_ACTION_NOTHING	BQ25186_I2C_BITMASK_NONE
#define BQ25186_PB_LPRESS_ACTION_RESET		BQ25186_I2C_BITMASK_3
#define BQ25186_PB_LPRESS_ACTION_SHIP		BQ25186_I2C_BITMASK_4
#define BQ25186_PB_LPRESS_ACTION_SHUTDOWN	BQ25186_I2C_BITMASK_4_3

#define BQ25186_WAKE1_TMR_300_MS			BQ25186_I2C_BITMASK_NONE
#define BQ25186_WAKE1_TMR_1_S				BQ25186_I2C_BITMASK_2

#define BQ25186_WAKE2_TMR_2_S				BQ25186_I2C_BITMASK_NONE
#define BQ25186_WAKE2_TMR_3_S				BQ25186_I2C_BITMASK_1

#define BQ25186_PUSH_DISABLED				BQ25186_I2C_BITMASK_NONE
#define BQ25186_PUSH_ENABLED				BQ25186_I2C_BITMASK_0

//Register 0x0a
#define BQ25186_SYS_REG_CTRL_BATTERY_TRACK	BQ25186_I2C_BITMASK_NONE
#define BQ25186_SYS_REG_CTRL_4_4V			BQ25186_I2C_BITMASK_5
#define BQ25186_SYS_REG_CTRL_4_5V			BQ25186_I2C_BITMASK_6
#define BQ25186_SYS_REG_CTRL_4_6V			BQ25186_I2C_BITMASK_6_5
#define BQ25186_SYS_REG_CTRL_4_7V			BQ25186_I2C_BITMASK_7
#define BQ25186_SYS_REG_CTRL_4_8V			BQ25186_I2C_BITMASK_75
#define BQ25186_SYS_REG_CTRL_4_9V			BQ25186_I2C_BITMASK_7_6
#define BQ25186_SYS_REG_CTRL_PASS_THROUGH	BQ25186_I2C_BITMASK_7_5

#define BQ25186_SYS_MODE_VIN_OR_VBAT		BQ25186_I2C_BITMASK_NONE
#define BQ25186_SYS_MODE_VBAT_ONLY			BQ25186_I2C_BITMASK_2

#define BQ25186_PG_GPO_HIGH_IMPEDENCE		BQ25186_I2C_BITMASK_NONE
#define BQ25186_PG_GPO_LOW					BQ25186_I2C_BITMASK_4

#define BQ25186_SYS_MODE_FLOAT				BQ25186_I2C_BITMASK_3
#define BQ25186_SYS_MODE_PULLDOWN			BQ25186_I2C_BITMASK_3_2

#define BQ25186_SYS_WATCHDOG_15S_ENABLE		BQ25186_I2C_BITMASK_1
#define BQ25186_SYS_WATCHDOG_15S_DISABLE	BQ25186_I2C_BITMASK_NONE

class bq25186 {

	public:
		bq25186();															//Constructor function
		~bq25186();															//Destructor function
		bool begin(TwoWire &i2cPort = Wire);								//Start the bq25186
		//Readable flags/status, see device datasheet for explanation of the values
		//Register 0x00
		uint8_t ts_open_stat();
		uint8_t chg_stat();													//Get charging status, returns BQ25186_ENABLED_BUT_NOT_CHARGING, BQ25186_CC_CHARGING, BQ25186_CV_CHARGING or BQ25186_CHARGING_DONE_OR_DISABLED
		uint8_t ilim_active_stat();
		uint8_t vdppm_active_stat();
		uint8_t vindpm_active_stat();
		uint8_t thermreg_active_stat();
		uint8_t vin_pgood_stat();
		//register 0x01
		uint8_t vin_ovp_stat();
		uint8_t buvlo_stat();
		uint8_t ts_stat();
		uint8_t safety_tmr_fault_flag();
		uint8_t wake1_flag();
		uint8_t wake2_flag();
		//Register 0x03
		uint8_t ts_fault();
		uint8_t ilim_active_flag();
		uint8_t vdppm_active_flag();
		uint8_t vindpm_active_flag();
		uint8_t thermreg_active_flag();
		uint8_t vin_ovp_fault_flag();
		uint8_t buvlo_fault_flag();
		uint8_t bat_ocp_fault();
		//Register 0x03
		uint8_t get_pg_pin_mode();
		bool set_pg_pin_mode(uint8_t value);
		float get_vbatreg();
		bool set_vbatreg(float voltage);
		//Register 0x04
		uint8_t get_chg_dis();
		bool set_chg_dis(uint8_t value);
		uint16_t get_ichg();
		bool set_ichg(uint16_t value);
		//Register 0x05
		uint8_t get_en_fc_mode();
		bool set_en_fc_mode(uint8_t value);
		uint8_t get_iprechg();
		bool set_iprechg(uint8_t value);
		uint8_t get_iterm();
		bool set_iterm(uint8_t value);
		uint8_t get_vindpm();
		bool set_vindpm(uint8_t value);
		uint8_t get_therm_reg();
		bool set_therm_reg(uint8_t value);
		//Register 0x06
		uint8_t get_ibat_ocp();
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
		uint8_t get_mr_reset_vin();
		bool set_mr_reset_vin(uint8_t value);
		uint8_t get_autowake();
		bool set_autowake(uint8_t value);
		uint8_t get_ilim();
		bool set_ilim(uint8_t value);
		//Register 0x09
		uint8_t get_reg_rst();
		bool set_reg_rst(uint8_t value);
		uint8_t get_reset_ship();
		bool set_reset_ship(uint8_t value);
		uint8_t get_long_press_action();
		bool set_long_press_action(uint8_t value);
		uint8_t get_wake1_tmr();
		bool set_wake1_tmr(uint8_t value);
		uint8_t get_wake2_tmr();
		bool set_wake2_tmr(uint8_t value);
		uint8_t get_en_push();
		bool set_en_push(uint8_t value);
		//Register 0xa0
		uint8_t get_sys_regulation_voltage();
		bool set_sys_regulation_voltage(uint8_t value);
		uint8_t get_pg_pin_state();
		bool set_pg_pin_state(uint8_t value);
		uint8_t get_sys_mode();
		bool set_sys_mode(uint8_t value);
		uint8_t get_i2c_watchdog_mode();
		bool set_i2c_watchdog_mode(uint8_t value);
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		void debug(Stream &);												//Start debugging on a stream
		void print_registers();												//Print all the registers to the debug Stream
		#endif
	protected:
	private:
		#if defined BQ25186_INCLUDE_DEBUG_FUNCTIONS
		Stream *debug_uart_ = nullptr;										//The stream used for the debugging
		void printHex(uint8_t value);										//Oh why am I doing this?
		void printHexln(uint8_t value);										//That's because I can't rely on the existence of printf in Arduino!
		void printBinary(uint8_t value);									//Even if we had printf we need these two!
		void printBinaryLn(uint8_t value);
		#endif
		TwoWire *i2cPort_ = nullptr;										//Pointer to I²C instance used by library
		const uint8_t bq25186_i2c_address_ = 0x6a;							//This can't be changed
		static const uint8_t bq25186_number_of_registers_ = 0x0d;
		bool bq25186_communicating_ok_ = false;
		uint8_t registers[bq25186_number_of_registers_];					//Storage for the BQ2518 registers
		uint32_t register_refresh_timer_ = 0;								//Rate limit register reads
		uint32_t register_refresh_rate_limit_ = 1e3;						//Rate limit defaults to once every 1000ms
		
		bool read_registers_(uint8_t start = 0x00, uint8_t length = 0x0d,	//Read registers, normally automatic before any other status command
			bool stop = true);
		uint8_t read_bitmasked_value_from_register_(uint8_t index,			//Read a bitmasked value from a register
			uint8_t mask);
		bool write_bitmasked_value_to_register_(uint8_t index, uint8_t mask,//Write a bitmasked value to a register
			uint8_t value);
		bool write_register_(uint8_t register, uint8_t value,				//Write a specific value to a specific register
			bool stop = true);
		bool auto_refresh_all_registers_();									//Automatic refresh of all registers before any action
};
#endif
