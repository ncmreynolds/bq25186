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

void bq25186::begin() {
	if(debug_uart_ != nullptr) {
		debug_uart_->println(F("BQ25186 library started"));
	}
}

void bq25186::debug(Stream &terminalStream) {
	debug_uart_ = &terminalStream;		//Set the stream used for the terminal
}

#endif
