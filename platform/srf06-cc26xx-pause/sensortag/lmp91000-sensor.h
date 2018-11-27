/*
 * Copyright (c) 2017, Holliot Incorporated - http://www.holliot.com/
 * All rights reserved.
 * Author: felix_peng
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup LMP91000 sensor
 * @{
 *
 * \file
 * Board-specific lmp91000 api for cc2650
 */
/*---------------------------------------------------------------------------*/
#ifndef LMP91000_SENSOR_H_
#define LMP91000_SENSOR_H_


#include "contiki-conf.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "lpm.h"
#include "rtimer.h"
#include "lib/sensors.h"
#include "sys/ctimer.h"

#include <stdbool.h>
/*---------------------------------------------------------------------------*/
/* Slave address */
#define LMP91000_I2C_ADDRESS                 0x48

/*---------------------------------------------------------------------------*/
/* register addresses */
#define LMP91000_REG_ADDR_STATUS		0x00
#define LMP91000_REG_ADDR_LOCK      	0x01
#define LMP91000_REG_ADDR_TIACN		0x10
#define LMP91000_REG_ADDR_REFCN     	0x11
#define LMP91000_REG_ADDR_MODCN         	0x12

/*---------------------------------------------------------------------------*/
/* TIACN register bit mask */
#define	BIT_MASK_ALL									0xFF
#define	TIACN_BIT_MASK_RLOAD							0x03
#define	TIACN_BIT_MASK_TIA_GAIN						0x1c


/*  REFCN register bit mask */
#define	REFCN_BIT_MASK_BIAS							0x0F
#define	REFCN_BIT_MASK_BIAS_SIGN						0x10
#define	REFCN_BIT_MASK_INT_Z							0x60
#define	REFCN_BIT_MASK_REF_SOURCE					0x80


/*  MODECN register bit mask*/
#define	MODECN_BIT_MASK_OP_MODE						0x07
#define	MODECN_BIT_MASK_FET_SHORT					0x80


/*---------------------------------------------------------------------------*/
/* STATUS register values */
#define STATUS_READY          		 0x01  /* Sensor on state */
#define STATUS_NOT_READY  	        0x00  /* Sensor off state */

#define LOCK_ON            			 0x01  /* Registers TIACN and REFCN in readonly mode (default) */
#define LOCK_OFF        			 0x00  /* Registers TIACN and REFCN in write mode */


/*---------------------------------------------------------------------------*/
/*  TIACN register: TIA_GAIN selection*/
#define TIACN_TIA_GAIN_EXT_RESIST			           0x00  /* default */
#define TIACN_TIA_GAIN_2K75					    0x04
#define TIACN_TIA_GAIN_3K5						    0x08
#define TIACN_TIA_GAIN_7K                  			    0x0c
#define TIACN_TIA_GAIN_14K					           0x10
#define TIACN_TIA_GAIN_35K					           0x14
#define TIACN_TIA_GAIN_120K					    0x18
#define TIACN_TIA_GAIN_350K					    0x1c

/* TIACN register: RLOAD  selection */
#define TIACN_RLOAD_10             					    0x00
#define TIACN_RLOAD_33               				    	    0x01
#define TIACN_RLOAD_50               				   	    0x02
#define TIACN_RLOAD_100               		  		    0x03  /* default */

/*---------------------------------------------------------------------------*/
/* REFCN register: BIAS selection */
#define REFCN_BIAS_0_PCT           		  	    0x00 /* default */
#define REFCN_BIAS_1_PCT           		  	    0x01
#define REFCN_BIAS_2_PCT            		    0x02
#define REFCN_BIAS_4_PCT            		    0x03
#define REFCN_BIAS_6_PCT            		    0x04
#define REFCN_BIAS_8_PCT           		    	    0x05
#define REFCN_BIAS_10_PCT           		    0x06
#define REFCN_BIAS_12_PCT            		    0x07
#define REFCN_BIAS_14_PCT            		    0x08
#define REFCN_BIAS_16_PCT            		    0x09
#define REFCN_BIAS_18_PCT            		    0x0A
#define REFCN_BIAS_20_PCT           		    0x0B
#define REFCN_BIAS_22_PCT            		    0x0C
#define REFCN_BIAS_24_PCT            		    0x0D

/* REFCN register: bias polarity selection */
#define REFCN_BIAS_SIGN_NEGATIVE              0x00 /* default */
#define REFCN_BIAS_SIGN_POSITIVE	           0x10

/* REFCN register: internal zero  selection */
#define REFCN_INT_Z_20_PCT          		    0x00 
#define REFCN_INT_Z_50_PCT           		    0x20 /* default */
#define REFCN_INT_Z_67_PCT          		    0x40 
#define REFCN_INT_Z_BYPASSED           	    0x60 /* only in O2 ground referred measurement */

/* REFCN register:reference voltage source  selection */
#define REFCN_REF_SOURCE_INT           	    0x00 /* default */
#define REFCN_REF_SOURCE_EXT           	    0x80

/*---------------------------------------------------------------------------*/
/* MODECN register: mode of operation selection */
#define MODECN_OP_MODE_DEEP_SLEEP           	    0x00 /* default */
#define MODECN_OP_MODE_2_LEAD           		    0x01
#define MODECN_OP_MODE_STANDBY           	    0x02
#define MODECN_OP_MODE_3_LEAD           		    0x03
#define MODECN_OP_MODE_TEMP_TIAOFF              0x06 /* STANDBY */
#define MODECN_OP_MODE_TEMP_TIAON          	    0x07 /* 3_LEAD */

/* MODECN register: shorting FET feature selection */
#define MODECN_FET_SHORT_DISABLED           	    0x00 /* default */
#define MODECN_FET_SHORT_ENABLED          	    0x80
/*---------------------------------------------------------------------------*/
/* Sensor data size */
#define DATA_SIZE                       16/*---------------------------------------------------------------------------*/
/* Slave address */
#define LMP91000_I2C_ADDRESS                 0x48

/*---------------------------------------------------------------------------*/
/* register addresses */
#define LMP91000_REG_ADDR_STATUS		0x00
#define LMP91000_REG_ADDR_LOCK      	0x01
#define LMP91000_REG_ADDR_TIACN		0x10
#define LMP91000_REG_ADDR_REFCN     	0x11
#define LMP91000_REG_ADDR_MODCN         	0x12

/*---------------------------------------------------------------------------*/
/* TIACN register bit mask */
#define	BIT_MASK_ALL									0xFF
#define	TIACN_BIT_MASK_RLOAD							0x03
#define	TIACN_BIT_MASK_TIA_GAIN						0x1c


/*  REFCN register bit mask */
#define	REFCN_BIT_MASK_BIAS							0x0F
#define	REFCN_BIT_MASK_BIAS_SIGN						0x10
#define	REFCN_BIT_MASK_INT_Z							0x60
#define	REFCN_BIT_MASK_REF_SOURCE					0x80


/*  MODECN register bit mask*/
#define	MODECN_BIT_MASK_OP_MODE						0x07
#define	MODECN_BIT_MASK_FET_SHORT					0x80


/*---------------------------------------------------------------------------*/
/* STATUS register values */
#define STATUS_READY          		 0x01  /* Sensor on state */
#define STATUS_NOT_READY  	        0x00  /* Sensor off state */

#define LOCK_ON            			 0x01  /* Registers TIACN and REFCN in readonly mode (default) */
#define LOCK_OFF        			 0x00  /* Registers TIACN and REFCN in write mode */


/*---------------------------------------------------------------------------*/
/*  TIACN register: TIA_GAIN selection*/
#define TIACN_TIA_GAIN_EXT_RESIST			           0x00  /* default */
#define TIACN_TIA_GAIN_2K75					    0x04
#define TIACN_TIA_GAIN_3K5						    0x08
#define TIACN_TIA_GAIN_7K                  			    0x0c
#define TIACN_TIA_GAIN_14K					           0x10
#define TIACN_TIA_GAIN_35K					           0x14
#define TIACN_TIA_GAIN_120K					    0x18
#define TIACN_TIA_GAIN_350K					    0x1c

/* TIACN register: RLOAD  selection */
#define TIACN_RLOAD_10             					    0x00
#define TIACN_RLOAD_33               				    	    0x01
#define TIACN_RLOAD_50               				   	    0x02
#define TIACN_RLOAD_100               		  		    0x03  /* default */

/*---------------------------------------------------------------------------*/
/* REFCN register: BIAS selection */
#define REFCN_BIAS_0_PCT           		  	    0x00 /* default */
#define REFCN_BIAS_1_PCT           		  	    0x01
#define REFCN_BIAS_2_PCT            		    0x02
#define REFCN_BIAS_4_PCT            		    0x03
#define REFCN_BIAS_6_PCT            		    0x04
#define REFCN_BIAS_8_PCT           		    	    0x05
#define REFCN_BIAS_10_PCT           		    0x06
#define REFCN_BIAS_12_PCT            		    0x07
#define REFCN_BIAS_14_PCT            		    0x08
#define REFCN_BIAS_16_PCT            		    0x09
#define REFCN_BIAS_18_PCT            		    0x0A
#define REFCN_BIAS_20_PCT           		    0x0B
#define REFCN_BIAS_22_PCT            		    0x0C
#define REFCN_BIAS_24_PCT            		    0x0D

/* REFCN register: bias polarity selection */
#define REFCN_BIAS_SIGN_NEGATIVE              0x00 /* default */
#define REFCN_BIAS_SIGN_POSITIVE	           0x10

/* REFCN register: internal zero  selection */
#define REFCN_INT_Z_20_PCT          		    0x00 
#define REFCN_INT_Z_50_PCT           		    0x20 /* default */
#define REFCN_INT_Z_67_PCT          		    0x40 
#define REFCN_INT_Z_BYPASSED           	    0x60 /* only in O2 ground referred measurement */

/* REFCN register:reference voltage source  selection */
#define REFCN_REF_SOURCE_INT           	    0x00 /* default */
#define REFCN_REF_SOURCE_EXT           	    0x80

/*---------------------------------------------------------------------------*/
/* MODECN register: mode of operation selection */
#define MODECN_OP_MODE_DEEP_SLEEP           	    0x00 /* default */
#define MODECN_OP_MODE_2_LEAD           		    0x01
#define MODECN_OP_MODE_STANDBY           	    0x02
#define MODECN_OP_MODE_3_LEAD           		    0x03
#define MODECN_OP_MODE_TEMP_TIAOFF              0x06 /* STANDBY */
#define MODECN_OP_MODE_TEMP_TIAON          	    0x07 /* 3_LEAD */

/* MODECN register: shorting FET feature selection */
#define MODECN_FET_SHORT_DISABLED           	    0x00 /* default */
#define MODECN_FET_SHORT_ENABLED          	    0x80
/*---------------------------------------------------------------------------*/
/* Sensor data size */
#define DATA_SIZE                       16

/*---------------------------------------------------------------------------*/
/*-----------------------------API------------------------------------------*/
/*---------------------------------------------------------------------------*/
bool lmp91000_read_reg(uint8_t *raw_data, uint8_t start_addr, uint8_t reg_len);

bool lmp91000_write_reg(uint8_t* w_data, uint8_t start_addr, uint8_t w_len);

/* when call this function, lmp91000 was set in 2_lead_potentiostat mode for sensors such as O2-A2. */
void lmp91000_init(void);
/*---------------------------------------------------------------------------*/
/* 	get raw data from ADC conversion, 
*	raw_data[0] = high byte
*	raw_data[1] = low byte
*	raw_data is temperature or gas concentration, defined by mode.
*/
uint8_t lmp91000_get_raw_data(uint8_t* raw_data);

/* get gas concentration */
uint8_t lmp91000_get_gas(double *gas);

/* get temperature */
uint8_t lmp91000_get_temp(double *temp);
/*---------------------------------------------------------------------------*/
/*-----------------------------API------------------------------------------*/
/*---------------------------------------------------------------------------*/
double lmp91000_gas_cal(uint8_t h_byte, uint8_t l_byte);

double lmp91000_temp_cal(uint8_t h_byte, uint8_t l_byte);

/*---------------------------------------------------------------------------*/
/* called to print register values */
void lmp91000_print_default(void);

/* configurations for O2 sensors in 2_lead_potentiostat_mode */
void lmp91000_2_lead_potentiostat_conf(void);

/* configurations for temperature_measure_with_tia_off mode */
void lmp91000_temp_measure_tia_off_conf(void);
/*---------------------------------------------------------------------------*/

/* O2 sensor calibration in fresh air */
void lmp91000_fresh_air_cali(void);

/* O2 sensor temperature compensation */
double lmp91000_temp_compensation(double gas);

/* O2 sensor self-check */
bool lmp91000_self_checking(void);
/*---------------------------------------------------------------------------*/
/** @} */

#endif


