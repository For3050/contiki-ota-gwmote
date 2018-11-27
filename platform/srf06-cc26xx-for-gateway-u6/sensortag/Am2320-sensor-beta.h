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
#ifndef AM2320_SENSOR_BETA_H_
#define AM2320_SENSOR_BETA_H_


#include "contiki-conf.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "lpm.h"
#include "rtimer.h"

#include <string.h>
#include <stdbool.h>


/*---------------------------------------------------------------------------*/
bool board_wakeup_am2320(void);

/*---------------------------------------------------------------------------*/
bool board_write_cmd(uint8_t func_code, uint8_t start_addr, uint8_t reg_len);

/*---------------------------------------------------------------------------*/
bool board_read_data(uint8_t * rdata, uint8_t rlen );

/*---------------------------------------------------------------------------*/
uint8_t am2320_get_raw_data(uint8_t *raw_data);

/*---------------------------------------------------------------------------*/
/** @} */

#endif



























