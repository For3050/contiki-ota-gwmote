/*
 * Copyright (c) 2017, Holliot: http://www.holliot.com/.
 * All rights reserved.
 *
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         spi driver and application API for cc2650
 * \author
 *         jerry wu <wuwei@holliot.com>
 */

#include "contiki.h"
#include "ti-lib.h"
#include <driverlib/ssi.h>
#include <stdbool.h>
#include <stdint.h>
#include <driverlib/interrupt.h>
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ssi.h"
#include "prcm.h"
#include "spi_driver.h"
#include "spi_api.h"

extern volatile uint8_t	spi_read_buf[R_BUFFER_SIZE];

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
  if(!(HWREG(PRCM_BASE + PRCM_O_SSICLKGR) & PRCM_SSICLKGR_CLK_EN_SSI0)) {
    return false;
  }

  return true;
}
/*---------------------------------------------------------------------------*/

void
spi_init(void)
{

	ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_SPI_FCSN);
  	ti_lib_gpio_set_dio(BOARD_IOID_SPI_FCSN);

	ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_SPI_UART);
  	ti_lib_gpio_set_dio(BOARD_IOID_SPI_UART);


	/* configure GPIO pin for communication with master  */
	ti_lib_ioc_port_configure_set(BOARD_IOID_IRQ_EXT, IOC_PORT_GPIO, IOC_STD_OUTPUT);
	ti_lib_ioc_io_port_pull_set(BOARD_IOID_IRQ_EXT, IOC_IOPULL_DOWN);

	spi_recv_event = process_alloc_event();/* alloc spi recv event */
	/* init spi slave */
	board_spi_open_slave( 4000000, BOARD_IOID_SPI_SCLK_TDI );
}



bool
spi_write(const uint8_t *buf, uint8_t len)
{
  if(accessible() == false) {
   return false;
  }


  /* first, send data length  */
  ti_lib_ssi_data_put(SSI0_BASE, len);

  /* pull up gpio pin to notify master that some data will be send */
  ti_lib_ioc_io_port_pull_set(BOARD_IOID_IRQ_EXT, IOC_IOPULL_UP);



  /* send data */
  while(len > 0) {
    ti_lib_ssi_data_put(SSI0_BASE, *buf);
    len--;
    buf++;
  }

  /* wait for all data had been send */
  while(!(HWREG(SSI0_BASE + SSI_O_SR) & SSI_SR_TFE)) ;

  /* pull down gpio pin after all data had been send */
  ti_lib_ioc_io_port_pull_set(BOARD_IOID_IRQ_EXT, IOC_IOPULL_DOWN);

  return true;
}

/*---------------------------------------------------------------------------*/

uint8_t
spi_read(uint8_t *buf)
{
  uint8_t index = 0;
  uint8_t rcvDataLen = 0;

  if(accessible() == false) {
    return 0;
  }

  rcvDataLen = spi_read_buf[0];

  //memcpy(buf, spi_read_buf+1, rcvDataLen);
  for( index = 0; index < rcvDataLen; index++ )
  	buf[index] = spi_read_buf[index+1];

  return rcvDataLen;
}

/*---------------------------------------------------------------------------*/
void
spi_close()
{
  /* Power down SSI0 */
  ti_lib_rom_prcm_peripheral_run_disable(PRCM_PERIPH_SSI0);
  ti_lib_prcm_load_set();
  while(!ti_lib_prcm_load_get());

  /* Restore pins to a low-consumption state */
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_MISO);
  ti_lib_ioc_io_port_pull_set(BOARD_IOID_SPI_MISO, IOC_IOPULL_DOWN);

  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_MOSI);
  ti_lib_ioc_io_port_pull_set(BOARD_IOID_SPI_MOSI, IOC_IOPULL_DOWN);

  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_SCLK_TDI);
  ti_lib_ioc_io_port_pull_set(BOARD_IOID_SPI_SCLK_TDI, IOC_IOPULL_DOWN);
}
/*---------------------------------------------------------------------------*/



