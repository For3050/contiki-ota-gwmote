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


#include <stdio.h>

volatile uint8_t spi_read_buf[R_BUFFER_SIZE] = { 0x00 };
static volatile uint32_t	rDataLen = 0;


PROCESS_NAME(spi_read_process);
process_event_t	spi_recv_event;

/*ssix interrupter handler function*/
void ssi_rx_handler(void)
{ 	
	ti_lib_ssi_int_clear(SSI0_BASE,CC26XX_SSI_INTERRUPT_ALL);
	for(uint8_t i = 0; ( HWREG(SSI0_BASE + SSI_O_SR) & SSI_SR_RNE ) != 0; i++ )
	{
		spi_read_buf[rDataLen++] = HWREG(SSI0_BASE + SSI_O_DR);
		if(rDataLen == spi_read_buf[0] + 1)
		{
			rDataLen = 0;
			if(spi_read_buf[0] > 0) /* receive data up to zero */
				process_post_synch(&spi_read_process, spi_recv_event, NULL);
			break;
		}
				
	}
}

/*enable ssi0 interrupter*/
void ssi_int_enable(void)
{

	ti_lib_ssi_int_clear(SSI0_BASE, CC26XX_SSI_INTERRUPT_ALL);
	ti_lib_ssi_int_disable(SSI0_BASE, CC26XX_SSI_INTERRUPT_ALL);
	ti_lib_ssi_int_enable(SSI0_BASE, CC26XX_SSI_RX_INTERRUPT_TRIGGERS);

	/* Acknowledge SSI interrupts */
	ti_lib_int_enable(INT_SSI0_COMB);

	ti_lib_ssi_int_register(SSI0_BASE,ssi_rx_handler);

}




void
board_spi_open_master(uint32_t bit_rate, uint32_t clk_pin)
{
  uint32_t buf;

  /* First, make sure the SERIAL PD is on */
  ti_lib_prcm_power_domain_on(PRCM_DOMAIN_SERIAL);
  while((ti_lib_prcm_power_domain_status(PRCM_DOMAIN_SERIAL)
         != PRCM_DOMAIN_POWER_ON));

  /* Enable clock in active mode */
  ti_lib_rom_prcm_peripheral_run_enable(PRCM_PERIPH_SSI0);
  ti_lib_prcm_load_set();
  while(!ti_lib_prcm_load_get());

  /* disable SPI before conf SPI */
  ti_lib_ssi_disable(SSI0_BASE);

  /* SPI configuration */
  ti_lib_ssi_int_disable(SSI0_BASE, SSI_RXOR | SSI_RXFF | SSI_RXTO | SSI_TXFF);
  ti_lib_ssi_int_clear(SSI0_BASE, SSI_RXOR | SSI_RXTO);
  ti_lib_rom_ssi_config_set_exp_clk(SSI0_BASE, ti_lib_sys_ctrl_clock_get(),
                                    SSI_FRF_MOTO_MODE_0,
                                    SSI_MODE_MASTER, bit_rate, SPI_DATA_WIDTH_8_BITS);
  ti_lib_rom_ioc_pin_type_ssi_master(SSI0_BASE, BOARD_IOID_SPI_MISO,
                                     BOARD_IOID_SPI_MOSI, BOARD_IOID_SPI_FSS_EXT, clk_pin);


  ti_lib_ssi_enable(SSI0_BASE);

  /* Get rid of residual data from SSI port */
  while(ti_lib_ssi_data_get_non_blocking(SSI0_BASE, &buf));
}


void
board_spi_open_slave(uint32_t bit_rate, uint32_t clk_pin)
{
  uint32_t buf;

  /* First, make sure the SERIAL PD is on */
  ti_lib_prcm_power_domain_on(PRCM_DOMAIN_SERIAL);
  while((ti_lib_prcm_power_domain_status(PRCM_DOMAIN_SERIAL)
         != PRCM_DOMAIN_POWER_ON));

  /* Enable clock in active mode */
  ti_lib_rom_prcm_peripheral_run_enable(PRCM_PERIPH_SSI0);
  ti_lib_prcm_load_set();
  while(!ti_lib_prcm_load_get());

  /* disable SPI before conf SPI */
  ti_lib_ssi_disable(SSI0_BASE);

  /* SPI configuration */
  ti_lib_ssi_int_disable(SSI0_BASE, SSI_RXOR | SSI_RXFF | SSI_RXTO | SSI_TXFF);
  ti_lib_ssi_int_clear(SSI0_BASE, SSI_RXOR | SSI_RXTO);
  ti_lib_rom_ssi_config_set_exp_clk(SSI0_BASE, ti_lib_sys_ctrl_clock_get(),
                                    SSI_FRF_MOTO_MODE_0,
                                    SSI_MODE_SLAVE, bit_rate, SPI_DATA_WIDTH_8_BITS);
  ti_lib_rom_ioc_pin_type_ssi_slave(SSI0_BASE, BOARD_IOID_SPI_MOSI,
                                     BOARD_IOID_SPI_MISO, BOARD_IOID_SPI_FSS_EXT, clk_pin);

  ssi_int_enable();

  ti_lib_ssi_enable(SSI0_BASE);

  /* Get rid of residual data from SSI port */
  while(ti_lib_ssi_data_get_non_blocking(SSI0_BASE, &buf));
}


/*---------------------------------------------------------------------------*/
