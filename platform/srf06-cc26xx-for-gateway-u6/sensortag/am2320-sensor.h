/*
 * Copyright (c) 2017, Holliot Incorporated - http://www.holliot.com/
 * All rights reserved.
 * Author: jerry_wu
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup AM2320-Aosong sensor
 * @{
 *
 * \file
 * Board-specific am2320 api for cc2650
 */
/*---------------------------------------------------------------------------*/
#ifndef AM2320_SENSOR_H_
#define AM2320_SENSOR_H_


#include "contiki-conf.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "lpm.h"
#include "rtimer.h"

#include <string.h>
#include <stdbool.h>








/*  */
bool board_wakeup_am2320(void);
/*---------------------------------------------------------------------------*/
/**
 * \brief Read the result register
 * \param raw_data Pointer to a buffer to store the reading
 * \return TRUE if valid data
 * \NOTE: raw_data buffer size should be at least reg_len +2, because the first 
 * two data are funciton code(0x03) and data length.
 */
bool
am2320_read_reg(uint8_t *raw_data, uint8_t start_addr, uint8_t reg_len);

void
am2320_write_reg(uint8_t* w_data, uint8_t start_addr, uint8_t w_len);

void am2320_get_humi_temp(double* humi, double* temp);

void am2320_get_raw_data(uint8_t *raw_data);

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/** @} */

#endif
