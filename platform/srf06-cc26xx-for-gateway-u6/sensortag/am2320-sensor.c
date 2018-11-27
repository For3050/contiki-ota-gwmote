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
#include "contiki-conf.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "lpm.h"
#include "rtimer.h"
#include "etimer.h"
#include <string.h>
#include <stdbool.h>
/*---------------------------------------------------------------------------*/
#define I2C_MAX_WAIT_TIME    (RTIMER_SECOND / 10)

#define LIMITED_BUSYWAIT(cond) do {                   \
    rtimer_clock_t end_time = RTIMER_NOW() + I2C_MAX_WAIT_TIME; \
    while(cond) {                                               \
      if(!RTIMER_CLOCK_LT(RTIMER_NOW(), end_time)) {            \
        return false;                                           \
      }                                                         \
    }                                                           \
  } while(0)
/*---------------------------------------------------------------------------*/
#define NO_INTERFACE 0xFF
/*---------------------------------------------------------------------------*/

#define		AM2320_I2C_ADDRESS		0x5C
#define		AM2320_WRITE_BUFFER_SIZE	0x10U
static uint8_t slave_addr = 0x5C;
static struct timer am2320_timer;

/*---------------------------------------------------------------------------*/

static bool
accessible(void)
{
  /* First, check the PD */
  if(ti_lib_prcm_power_domain_status(PRCM_DOMAIN_SERIAL)
     != PRCM_DOMAIN_POWER_ON) {
    return false;
  }

  /* Then check the 'run mode' clock gate */
  if(!(HWREG(PRCM_BASE + PRCM_O_I2CCLKGR) & PRCM_I2CCLKGR_CLK_EN)) {
    return false;
  }

  return true;
}

static bool
i2c_status()
{
  uint32_t status;

  status = ti_lib_i2c_master_err(I2C0_BASE);
  if(status & (I2C_MSTAT_DATACK_N_M | I2C_MSTAT_ADRACK_N_M)) {
    ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
  }

  return status == I2C_MASTER_ERR_NONE;
}

/* this function is different from the board_i2c_read in board_i2c.c
 * add a 30us delay after start transfer, because reading am2320 should delay at 
 * least 30us after the completion of I2C address.
 */
static bool
board_am2320_i2c_read(uint8_t *data, uint8_t len)
{
  uint8_t i;
  bool success;
  uint32_t delay=1000;
  /* Set slave address */
  ti_lib_i2c_master_slave_addr_set(I2C0_BASE, slave_addr, true);

  /* Check if another master has access */
  LIMITED_BUSYWAIT(ti_lib_i2c_master_bus_busy(I2C0_BASE));

  /* Assert RUN + START + ACK */
  ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

  while(delay--);

  i = 0;
  success = true;
  while(i < (len - 1) && success) {
    LIMITED_BUSYWAIT(ti_lib_i2c_master_busy(I2C0_BASE));
    success = i2c_status();
    if(success) {
      data[i] = ti_lib_i2c_master_data_get(I2C0_BASE);
      ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
      i++;
    }
  }

  if(success) {
    ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    LIMITED_BUSYWAIT(ti_lib_i2c_master_busy(I2C0_BASE));
    success = i2c_status();
    if(success) {
      data[len - 1] = ti_lib_i2c_master_data_get(I2C0_BASE);
      LIMITED_BUSYWAIT(ti_lib_i2c_master_bus_busy(I2C0_BASE));
    }
  }

  return success;
}





static void
select_on_bus(void)
{
  /* Select slave and set clock rate */
  board_i2c_select(BOARD_I2C_INTERFACE_0, AM2320_I2C_ADDRESS);
}

/*  */
bool board_wakeup_am2320(void)
{
	board_i2c_wakeup();/* max speed: 100Kbps */

	select_on_bus();
	/* wait > 800us */
	/* this should be check */
	/* Write slave address */
	ti_lib_i2c_master_slave_addr_set(I2C0_BASE, slave_addr, false);

	/* Check if another master has access */
	LIMITED_BUSYWAIT(ti_lib_i2c_master_bus_busy(I2C0_BASE));

	/* Assert RUN + START + STOP */
	ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
	LIMITED_BUSYWAIT(ti_lib_i2c_master_busy(I2C0_BASE));

	return true;

}

/*---------------------------------------------------------------------------*/
/**
 * \brief Read the result register
 * \param raw_data Pointer to a buffer to store the reading
 * \return TRUE if valid data
 * \NOTE: raw_data buffer size should be at least reg_len +2, because the first 
 * two data are funciton code(0x03) and data length.
 */
bool
am2320_read_reg(uint8_t *raw_data, uint8_t start_addr, uint8_t reg_len)
{

  uint8_t read_cmd[3]={0x03, start_addr,reg_len};



  select_on_bus();
  
  board_i2c_write(read_cmd,3);

  /* after send r/w cmd, host should wait at least 1.5ms */
  timer_set(&am2320_timer, CLOCK_SECOND >> 8);
  while( !(timer_expired(&am2320_timer)) );

  /* host read back data: function code(0x03) + data length + read data + 2 Bytes CRC code */
  board_am2320_i2c_read(raw_data, reg_len + 4);


  return true;
}

void
am2320_write_reg(uint8_t* w_data, uint8_t start_addr, uint8_t w_len)
{
	uint8_t w_cmd_data[AM2320_WRITE_BUFFER_SIZE]={0x10, start_addr, w_len,};

	memcpy(w_cmd_data+3, w_data, w_len);

	board_i2c_write(w_cmd_data, w_len+3);

	/* should be check whether am2320 sensor response message have to be read or not. */
}


double
calculate_humi_temp(uint8_t high_byte, uint8_t low_byte)
{
	uint32_t t = high_byte * 256 + (low_byte >> 4 ) * 16 + (low_byte & 0x0F);
	return t * 0.1;
}



void am2320_get_humi_temp(double* humi, double* temp)
{
	/* function code + data length + raw data + 2 bytes crc code */
	uint8_t r_temp_humi[8]={0x00,};
	board_wakeup_am2320();
  	am2320_read_reg(r_temp_humi, 0x00, 0x04);
	*humi = calculate_humi_temp(r_temp_humi[2],r_temp_humi[3]);
	*temp = calculate_humi_temp(r_temp_humi[4],r_temp_humi[5]);
}

void am2320_get_raw_data(uint8_t *raw_data)
{
	/* function code + data length + raw data + 2 bytes crc code */
	uint8_t r_temp_humi[8]={0x00,};
	board_wakeup_am2320();
  	am2320_read_reg(r_temp_humi, 0x00, 0x04);
	raw_data[0] = r_temp_humi[2];  //humi high_byte
	raw_data[1] = r_temp_humi[3];  //humi low_byte
	raw_data[2] = r_temp_humi[4];  //temp high_byte
	raw_data[3] = r_temp_humi[5];  //temp low_byte
}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/** @} */
