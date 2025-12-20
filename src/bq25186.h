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
#include <Arduino.h>

class bq25186 {

	public:
		bq25186();														//Constructor function
		~bq25186();														//Destructor function
		void begin();													//Start the bq25186
		void debug(Stream &);											//Start debugging on a stream
	protected:
	private:
		Stream *debug_uart_ = nullptr;									//The stream used for the debugging
};
#endif
