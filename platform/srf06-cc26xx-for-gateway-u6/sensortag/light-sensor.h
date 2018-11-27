/*
 * Copyright (c) 2017, Holliot Incorporated - http://www.holliot.com/
 * All rights reserved.
 * Author: jerry_wu
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup LIGHT sensor
 * @{
 *
 * \file
 * Board-specific light sensor api for cc2650
 */
/*---------------------------------------------------------------------------*/

#ifndef 	__LIGHT_SENSOR_H_
#define 	__LIGHT_SENSOR_H_

#include <stdbool.h>


extern bool light_flag;

extern void light_sensor_init(void);


#endif
