/*
 * Copyright (c) 2017, Holliot Incorporated - http://www.holliot.com/
 * All rights reserved.
 * Author: jerry_wu
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup PIR sensor
 * @{
 *
 * \file
 * Board-specific PIR api for cc2650
 */
/*---------------------------------------------------------------------------*/
#include "contiki-conf.h"
#include "ti-lib.h"
#include <stdbool.h>
#include <stdio.h>
#include "pir-sensor.h"

bool pir_flag = false;

static void
pir_interrupt_handler(void)
{
	ti_lib_ioc_int_disable(BOARD_IOID_PIR);
	pir_flag = true;
}

/* NOTE:1.register interrupt method; 2. the order of register/enable interrupt */
void pir_init(void)
{
	ti_lib_ioc_int_disable(BOARD_IOID_PIR);
	ti_lib_ioc_int_clear(BOARD_IOID_PIR);
	ti_lib_gpio_clear_event_dio(BOARD_IOID_PIR);

	ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_PIR);

	ti_lib_ioc_int_enable(BOARD_IOID_PIR);
	ti_lib_ioc_io_int_set(BOARD_IOID_PIR, IOC_INT_ENABLE, IOC_RISING_EDGE);
	//gpio_interrupt_register_handler(BOARD_IOID_PIR,pir_interrupt_handler);
	ti_lib_ioc_int_register(pir_interrupt_handler);
	ti_lib_ioc_int_enable(BOARD_IOID_PIR);
	ti_lib_int_enable(INT_AON_GPIO_EDGE);
	
}
