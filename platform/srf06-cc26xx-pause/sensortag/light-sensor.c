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
#include "contiki-conf.h"
#include "ti-lib.h"
#include <stdbool.h>
#include <stdio.h>
#include "light-sensor.h"

bool light_flag = false;

static void
light_interrupt_handler(void)
{
	ti_lib_ioc_int_disable(BOARD_IOID_LIGHT_SNSR);
	light_flag = true;
}

/* NOTE:1.register interrupt method; 2. the order of register/enable interrupt */
void light_sensor_init(void)
{
	ti_lib_ioc_int_disable(BOARD_IOID_LIGHT_SNSR);
	ti_lib_ioc_int_clear(BOARD_IOID_LIGHT_SNSR);
	ti_lib_gpio_clear_event_dio(BOARD_IOID_LIGHT_SNSR);

	ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_LIGHT_SNSR);

	ti_lib_ioc_int_enable(BOARD_IOID_LIGHT_SNSR);
	ti_lib_ioc_io_int_set(BOARD_IOID_LIGHT_SNSR, IOC_INT_ENABLE, IOC_BOTH_EDGES);
	//gpio_interrupt_register_handler(BOARD_IOID_LIGHT_SNSR,light_interrupt_handler);
	ti_lib_ioc_int_register(light_interrupt_handler);
	ti_lib_ioc_int_enable(BOARD_IOID_LIGHT_SNSR);
	ti_lib_int_enable(INT_AON_GPIO_EDGE);
	
}
