/*
 * Copyright (c) 2017, Holliot Incorporated - http://www.holliot.com/
 * All rights reserved.
 * Author: jerry_wu
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup analog temperature sensor
 * @{
 *
 * \file
 * Board-specific analog temperature sensor api for cc2650
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/sensors.h"
#include "srf06/als-sensor.h"
#include "sys/timer.h"
#include "dev/adc-sensor.h"
#include "dev/aux-ctrl.h"

#include "ti-lib.h"

#include <stdint.h>


uint16_t analog_read_temp(void)
{
	uint16_t single_adc_sample=0;
	/* intialisation of ADC */
	ti_lib_aon_wuc_aux_wakeup_event(AONWUC_AUX_WAKEUP);
	while(!(ti_lib_aon_wuc_power_status_get() & AONWUC_AUX_POWER_ON));

	/*
	* Enable clock for ADC digital and analog interface (not currently enabled
	* in driver)
	*/
	ti_lib_aux_wuc_clock_enable(AUX_WUC_ADI_CLOCK | AUX_WUC_ANAIF_CLOCK |
		                AUX_WUC_SMPH_CLOCK);

	while(ti_lib_aux_wuc_clock_status(AUX_WUC_ADI_CLOCK | AUX_WUC_ANAIF_CLOCK |
		                      AUX_WUC_SMPH_CLOCK) != AUX_WUC_CLOCK_READY);


	ti_lib_rom_ioc_pin_type_gpio_input(BOARD_IOID_ANA_TEMP);

	ti_lib_gpio_set_dio(BOARD_IOID_ANA_TEMP);

	/* Connect AUX IO6 (DIO24) as analog input. */
	ti_lib_aux_adc_select_input(ADC_COMPB_IN_AUXIO6);

	//ti_lib_aux_adc_enable_async(AUXADC_REF_FIXED, AUXADC_TRIGGER_MANUAL);
	/* Trigger ADC converting */
	ti_lib_aux_adc_enable_sync(AUXADC_REF_VDDS_REL, AUXADC_SAMPLE_TIME_2P7_US,
		     AUXADC_TRIGGER_MANUAL);
	ti_lib_aux_adc_gen_manual_trigger();
	clock_delay_usec(100);
	single_adc_sample = ti_lib_aux_adc_read_fifo();
	ti_lib_aux_adc_disable();

	return single_adc_sample;

}



