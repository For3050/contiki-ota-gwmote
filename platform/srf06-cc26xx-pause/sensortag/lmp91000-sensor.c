/*
 * Copyright (c) 2014, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup sensortag-cc26xx-gas-sensor
 * @{
 *
 * \file
 *  Driver for the Sensortag  LMP9100 sensor
 */
/*---------------------------------------------------------------------------*/
#include "lmp91000-sensor.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
	/* LMP91000 CONFIGURATION */
/*---------------------------------------------------------------------------*/
static struct timer lmp91000_timer;
static double temp = 0.0;
static double i_fresh = 91;

/*---------------------------------------------------------------------------*/
#define I2C_MAX_WAIT_TIME    (RTIMER_SECOND / 10)

#define NO_INTERFACE 0xFF

#define LIMITED_BUSYWAIT(cond) do {                   \
    rtimer_clock_t end_time = RTIMER_NOW() + I2C_MAX_WAIT_TIME; \
    while(cond) {                                               \
      if(!RTIMER_CLOCK_LT(RTIMER_NOW(), end_time)) {            \
        return false;                                           \
      }                                                         \
    }                                                           \
  } while(0)
/*---------------------------------------------------------------------------*/
bool lmp91000_notify_ready(void)
{
 	  uint8_t status = 0;
	
	  /*  Check the status of lmp91000 */
	  lmp91000_read_reg(&status, LMP91000_REG_ADDR_STATUS, 1);
	  if(status == STATUS_NOT_READY)
	    return false;
	  
	  return true;
}
/*---------------------------------------------------------------------------*/
static void
select_on_bus(void)
{
  /* Select slave and set clock rate */
#if 0
  ti_lib_rom_ioc_pin_type_gpio_output(BOARD_IOID_MENB);			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  ti_lib_gpio_clear_dio(BOARD_IOID_MENB);
  clock_delay_usec(1);
#endif
  board_i2c_select(BOARD_I2C_INTERFACE_0, LMP91000_I2C_ADDRESS);
}

/*---------------------------------------------------------------------------*/
  /* Disable lmp91000 by set MENB high */
void lmp91000_deselect(void)
{
	clock_delay_usec(1);
	
#if 0
	ti_lib_gpio_set_dio(BOARD_IOID_MENB);
#endif
}
/*---------------------------------------------------------------------------*/
bool
lmp91000_read_reg(uint8_t *raw_data, uint8_t start_addr, uint8_t reg_len)
{
  uint8_t success = 0x00;
  select_on_bus();
  while(!lmp91000_notify_ready());

  success = board_i2c_write_read(&start_addr, 1, raw_data, reg_len);
  lmp91000_deselect();
  return success;
}

/*---------------------------------------------------------------------------*/
bool
lmp91000_write_reg(uint8_t *w_data, uint8_t start_addr, uint8_t w_len)
{
	uint8_t success = 0x00;
	uint8_t data[DATA_SIZE] = {start_addr, };

	memcpy(data + 1, w_data, w_len);
	select_on_bus();
	while(!lmp91000_notify_ready());
	
	success  = board_i2c_write(data, w_len+1);
	lmp91000_deselect();
	return success;
}

/*---------------------------------------------------------------------------*/
  /* Set write_protection for TIACN and REFCN registers  */
  /* pra status = 1(default), Registers TIACN, REFCN in read only mode (default)  */
  /* pra status = 0,  Registers TIACN, REFCN in write mode */

bool set_write_protection(uint8_t status)
{
	uint8_t success = 0;
	/* parameter check */
	if((status == LOCK_ON ) ||(status == LOCK_OFF ))
		{
			lmp91000_write_reg(&status, LMP91000_REG_ADDR_LOCK, 1);
			success = 1;
		}
	return success;
}
/*---------------------------------------------------------------------------*/
  /* configure register bits domain  */

bool lpm91000_reg_conf(uint8_t reg_addr, uint8_t bit_mask, uint8_t selection)
{
	uint8_t success = 0x00;
	uint8_t value = 0x00;

	select_on_bus();
	set_write_protection(LOCK_OFF);
	
	lmp91000_read_reg(&value, reg_addr, 1);
	value = ((value & (~bit_mask)) | selection);
	
	success = lmp91000_write_reg(&value, reg_addr, 1);

	set_write_protection(LOCK_ON);
	return success;
}

/*---------------------------------------------------------------------------*/
/* display register value  */
void lmp91000_print_default(void)
{
	uint8_t r_buf[4] = {0x00,};
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_STATUS, 2);
	printf("STATUS: default = 0x00, real = %x\n", r_buf[0]);
	printf("LOCK    : default = 0x01, real = %x\n", r_buf[1]);

	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_TIACN, 3);
	printf("TIACN   : default = 0x03, real = %x\n", r_buf[0]);	
	printf("REFCN   : default = 0x20, real = %x\n", r_buf[1]);
	printf("MODECN: default = 0x00, real = %x\n", r_buf[2]);
}

/*---------------------------------------------------------------------------*/
   /* 2_lead_in_potentiostat_configuration */
void lmp91000_2_lead_potentiostat_conf(void)
{
	uint8_t r_buf[4] = {0x00, };
	uint8_t w_buf = 0x00;
	
	  /* for test  */
	lmp91000_print_default();
	
	  /* 1,LOCK REG:turn off write_protection on TIACN\REFCN  */
	w_buf = LOCK_OFF;
	lmp91000_write_reg(&w_buf, LMP91000_REG_ADDR_LOCK, 1);
	 /* for test  */ 
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_LOCK, 1);
	printf("LOCK: write = 0x00, read = %x\n", r_buf[0]);

	  /* 2, MODECN REG:select mode 0x03;
	  				   FET_SHORT disabled(default), 0x00*/
	w_buf = (MODECN_OP_MODE_3_LEAD | MODECN_FET_SHORT_DISABLED);
	lmp91000_write_reg(&w_buf, LMP91000_REG_ADDR_MODCN, 1);	
	 /* for test  */ 
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_MODCN, 1);
	printf("MODE: write = 0x03, read = %x\n", r_buf[0]);

	  /* 3, TIACN REG: RLOAD = 100 ohm, 
	  				 TIA_GAIN = 7 Kohm */
	w_buf = (TIACN_TIA_GAIN_7K | TIACN_RLOAD_100);
	lmp91000_write_reg(&w_buf, LMP91000_REG_ADDR_TIACN, 1);	
	 /* for test  */ 
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_TIACN, 1);
	printf("TIACN: write = 0x0F, read = %x\n", r_buf[0]);

	  /* 4, REFCN REG: REF_SOURCE:external(default) 0x80; 
	  				 REFCN_INT_Z_67_PCT, 0x40;  
	  				 BIAS_SIGN:positive, 0x10; 		
	  				 BIAS:0%(default), 0x00 */	  
	w_buf = (REFCN_REF_SOURCE_EXT | REFCN_BIAS_SIGN_POSITIVE | REFCN_INT_Z_67_PCT | REFCN_BIAS_0_PCT);
	lmp91000_write_reg(&w_buf, LMP91000_REG_ADDR_REFCN, 1);	
	 /* for test  */ 
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_REFCN, 1);
	printf("REFCN: write = 0xD0, read = %x\n", r_buf[0]);
	

	  /*5,turn on write_protection on TIACN\REFCN  */
	w_buf = LOCK_ON;
	lmp91000_write_reg(&w_buf, LMP91000_REG_ADDR_LOCK, 1);	  
	 /* for test  */ 
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_LOCK, 1);
	printf("LOCK: write = 0x01, read = %x\n", r_buf[0]);	
	
}

/*---------------------------------------------------------------------------*/
   /* Temperature measurement(TIA OFF) mode configuration */
void lmp91000_temp_measure_tia_off_conf(void)
{
	  /*  MODECN REG:select mode 0x06;
	  				   FET_SHORT disabled(default), 0x00*/
	uint8_t w_buf = 0x00;
	uint8_t r_buf[4] = {0x00, };
	w_buf = (MODECN_OP_MODE_TEMP_TIAOFF | MODECN_FET_SHORT_DISABLED);
	lmp91000_write_reg(&w_buf, LMP91000_REG_ADDR_MODCN, 1);	
	 /* for test  */ 
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_MODCN, 1);
	printf("MODE: write = 0x06, read = %x\n", r_buf[0]);

}
/*---------------------------------------------------------------------------*/
void lmp91000_init(void)
{
	lmp91000_2_lead_potentiostat_conf();
}

/*---------------------------------------------------------------------------*/
	/* ADC CONVERTION and CACULATION*/
/*---------------------------------------------------------------------------*/
/* ADC CONVERT */
uint8_t lmp91000_get_raw_data(uint8_t* raw_data)		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

	/* Connect AUX IO6 (DIO24) as analog input. */
	ti_lib_aux_adc_select_input(ADC_COMPB_IN_AUXIO6);		//!!!!!!!!!!!!!!!!!DIO 25

	//ti_lib_aux_adc_enable_async(AUXADC_REF_FIXED, AUXADC_TRIGGER_MANUAL);
	/* Trigger ADC converting */
	ti_lib_aux_adc_enable_sync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US,
		     AUXADC_TRIGGER_MANUAL);

	ti_lib_aux_adc_gen_manual_trigger();
	clock_delay_usec(10);

	single_adc_sample = ti_lib_aux_adc_read_fifo();

	ti_lib_aux_adc_disable();

	*raw_data = single_adc_sample >> 8; /* high byte */
	*(raw_data+1) = single_adc_sample & 0xFF; /* low byte */

	return 1;

}

/*---------------------------------------------------------------------------*/
/* temperature calculation */
double lmp91000_temp_cal(uint8_t h_byte, uint8_t l_byte)
{
	uint8_t r_buf[2] = {0x00, };
	
	/* check mode first, then determin to do calculation or not */
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_MODCN, 1);
	if((r_buf[0] & MODECN_BIT_MASK_OP_MODE)!= MODECN_OP_MODE_TEMP_TIAOFF)
		return 0;
	/* Caution on temprature under 0   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! undefined */

	uint32_t decimal = (h_byte&0x0f) * 256 + (l_byte>>4)*16 + (l_byte & 0x0f);
	

	double vout = decimal * 1.221;	//unit: mV

	//temp = (1562.2 - vout)/8.16;	//formula
	temp = 191.4 - vout*0.1225;	//formula
	
	return temp;
}
/*---------------------------------------------------------------------------*/
/* return temperature */
uint8_t lmp91000_get_temp(double *temp)
{
	static uint8_t raw_data[2] = {0x00, 0x00};
	lmp91000_temp_measure_tia_off_conf();
 	lmp91000_get_raw_data(raw_data);
	*temp = lmp91000_temp_cal(raw_data[0], raw_data[1]);
	
	return 1;
}
/*---------------------------------------------------------------------------*/
/*  Iwe caculation */
double lmp91000_iwe_cal(uint8_t h_byte, uint8_t l_byte)
{
	uint8_t r_buf[2] = {0x00, };
	
	/* check mode first, then determin to do calculation or not */
	lmp91000_read_reg(r_buf, LMP91000_REG_ADDR_MODCN, 1);
	if((r_buf[0] & MODECN_BIT_MASK_OP_MODE)!= MODECN_OP_MODE_3_LEAD)
		return 0;

	uint32_t decimal = h_byte * 256 + (l_byte>>4)*16 + (l_byte & 0x0f);
	
	double vout = decimal * 1.221;	//unit: mV
	
	//double iwe = (1670 - vout)/7;	//unit: uA
	double iwe = 238.6 - 0.143*vout;	//unit: uA

	return iwe;
}
/*---------------------------------------------------------------------------*/
/*  gas concentration caculation */
double lmp91000_gas_cal(uint8_t h_byte, uint8_t l_byte)
{
	double iwe = lmp91000_iwe_cal(h_byte, l_byte);
	double gas = 20.9 * (iwe/i_fresh);	//unit: %

	return gas;
}
/*---------------------------------------------------------------------------*/
/* return gas concentration */
uint8_t lmp91000_get_gas(double *gas)
{
	static uint8_t raw_data[2] = {0x00, 0x00};
	lmp91000_2_lead_potentiostat_conf();
	lmp91000_get_raw_data(raw_data);
	*gas = lmp91000_gas_cal(raw_data[0], raw_data[1]);

	return 1;
}

/*---------------------------------------------------------------------------*/
	/* CALIBRATION */
/*---------------------------------------------------------------------------*/
double absolute(double a, double b)
{
	double result = 0.0;
	if(a > b)
		result = a-b;
	else
		result = b-a;
	return result;
}


/* After the sensor is stable, this function is called to calibrate i_cali in condition T = 25, fresh air */
void lmp91000_fresh_air_cali(void)
{
	static uint8_t raw_data[2] = {0x00, };
	static double iwe1 = 0.0;
	static double iwe2 = 0.0;
	double result = 0.0;
	
	lmp91000_init();

	do{
		printf("Calibrating......");				
		lmp91000_get_raw_data(raw_data);
		iwe1 = lmp91000_iwe_cal(raw_data[0], raw_data[1]);	
		
  		timer_set(&lmp91000_timer, 3 * CLOCK_SECOND);
  		while( !(timer_expired(&lmp91000_timer)) );
		
		lmp91000_get_raw_data(raw_data);
		iwe2 = lmp91000_iwe_cal(raw_data[0], raw_data[1]);			
		}while((result = absolute(iwe2, iwe1)) > 1);	

	if((iwe1 > 80) && (iwe1 < 120))
		i_fresh = iwe1;
	else
		printf("lmp9100_fresh_air_calibration FAILED!");
		
}
/*---------------------------------------------------------------------------*/
/*Call double lmp91000_temp_cal(uint8_t h_byte, uint8_t l_byte) first before call this function  */
double lmp91000_temp_compensation(double gas)
{
	double gas_cali = 0.0;
	if(temp >= 20)
		{
			gas_cali  = gas - 0.2*(temp -20);	//unit: %
		}
	else 
		gas_cali  = gas + 0.2*(20 - temp);	//unit: %

	return gas_cali;
}
/*---------------------------------------------------------------------------*/
	/* SELF-CHECKING */
/*---------------------------------------------------------------------------*/
bool lmp91000_self_checking(void)
{
	return 1;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/** @} */

