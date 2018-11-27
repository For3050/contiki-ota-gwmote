
#include "contiki-conf.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "lpm.h"
#include "rtimer.h"
#include "etimer.h"
#include <string.h>
#include <stdbool.h>
#include "Am2320-sensor-beta.h"

/*---------------------------------------------------------------------------*/
#define	AM2320_SLAVE_ADDR	0x5c

#define	AM2320_FUNC_READ_MUTI_REG	0X03

#define	AM2320_FUNC_WRITE_MUTI_REG		0X10


#define	AM2320_REG_ADDR_HUMI_H	0X00

#define	AM2320_REG_ADDR_HUMI_L	0X01

#define	AM2320_REG_ADDR_TEMP_H	0X02

#define	AM2320_REG_ADDR_TEMP_L	0X03

#define	MAX_SIZE	10
/*---------------------------------------------------------------------------*/
//static uint8_t slave_addr = 0xb8;
static uint8_t slave_addr = 0x5C;
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
static bool accessible(void)
{
	if(ti_lib_prcm_power_domain_status(PRCM_DOMAIN_SERIAL)
			!= PRCM_DOMAIN_POWER_ON)
	{
		return false;
	}
	if(!(HWREG(PRCM_BASE + PRCM_O_I2CCLKGR) & PRCM_I2CCLKGR_CLK_EN))
	{
		return false;
	}
	return true;
}
/*---------------------------------------------------------------------------*/
static bool i2c_status()
{
	uint32_t status;
	status = ti_lib_i2c_master_err(I2C0_BASE);
	if(status & (I2C_MSTAT_DATACK_N_M | I2C_MSTAT_ADRACK_N_M))
	{
		ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
	}
	return status == I2C_MASTER_ERR_NONE;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
bool
board_i2c_write_delay_us(uint8_t *data, uint8_t len, uint16_t us)
{
  uint32_t i;
  bool success;

  /* Write slave address */
  ti_lib_i2c_master_slave_addr_set(I2C0_BASE, slave_addr, false);

  /* Write first byte */
  ti_lib_i2c_master_data_put(I2C0_BASE, data[0]);

  /* Check if another master has access */
  LIMITED_BUSYWAIT(ti_lib_i2c_master_bus_busy(I2C0_BASE));

  /* Assert RUN + START */
  ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  clock_delay_usec(us);
  
  LIMITED_BUSYWAIT(ti_lib_i2c_master_busy(I2C0_BASE));
  success = i2c_status();

  for(i = 1; i < len && success; i++) {
    /* Write next byte */
    ti_lib_i2c_master_data_put(I2C0_BASE, data[i]);
    if(i < len - 1) {
      /* Clear START */
      ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
      LIMITED_BUSYWAIT(ti_lib_i2c_master_busy(I2C0_BASE));
      success = i2c_status();
    }
  }

  /* Assert stop */
  if(success) {
    /* Assert STOP */
    ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    LIMITED_BUSYWAIT(ti_lib_i2c_master_busy(I2C0_BASE));
    success = i2c_status();
    LIMITED_BUSYWAIT(ti_lib_i2c_master_bus_busy(I2C0_BASE));
  }

  return success;
}

/*---------------------------------------------------------------------------*/

bool
board_i2c_read_delay_us(uint8_t *data, uint8_t len, uint16_t us)
{
  uint8_t i;
  bool success;

  /* Set slave address */
  ti_lib_i2c_master_slave_addr_set(I2C0_BASE, slave_addr, true);

  /* Check if another master has access */
  LIMITED_BUSYWAIT(ti_lib_i2c_master_bus_busy(I2C0_BASE));

  /* Assert RUN + START + ACK */
  ti_lib_i2c_master_control(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

  clock_delay_usec(us);

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
/*---------------------------------------------------------------------------*/
bool
board_i2c_write_none_single(void)	//add by felix peng
{
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
/*rlen = 1byte(func code) + 1byte(length) + reg_len*/
bool board_read_data(uint8_t * rdata, uint8_t rlen )
{	
	board_i2c_read_delay_us(rdata, rlen, 30);
	return 1;
}

/*---------------------------------------------------------------------------*/

bool board_write_cmd(uint8_t func_code, uint8_t start_addr, uint8_t reg_len)
{
	uint8_t success; 
	uint8_t r_cmd[3] = {func_code, start_addr, reg_len};
	board_i2c_select(BOARD_I2C_INTERFACE_0, AM2320_SLAVE_ADDR);

	success = board_i2c_write(r_cmd, 3);
	
	clock_delay_usec(1500);

	return success;
}


/*---------------------------------------------------------------------------*/
bool board_wakeup_am2320(void)
{
	board_i2c_wakeup();
	board_i2c_select(BOARD_I2C_INTERFACE_0, AM2320_SLAVE_ADDR);
	clock_delay_usec(1000);	
	board_i2c_write_none_single();

	return 1;
}
/*---------------------------------------------------------------------------*/
uint8_t am2320_get_raw_data(uint8_t * raw_data)
{	
	uint8_t rdata[MAX_SIZE] = {0x0, };
	uint8_t rlen = 0x0;
	uint8_t i = 0;

	board_wakeup_am2320();
	
	board_write_cmd(AM2320_FUNC_READ_MUTI_REG, 0x00, 0x04);
	
	rlen = 2 + 4;
	board_read_data(rdata, rlen);
	for(i = 0; i < 6; i++)
		{
			//raw_data[i] = rdata[i+2];
			raw_data[i] = rdata[i];
		}		
	
	return 1;	
}

#if 0
void am2320_caculate_temp_humi(uint8_t *raw_data)
{
	uint16_t humi_data = raw_data[0]<<8 || raw_data[1];
	uint16_t temp_data = raw_data[2]<<8 || raw_data[3];
}
#endif
