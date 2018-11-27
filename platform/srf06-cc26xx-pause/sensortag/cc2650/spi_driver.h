/*
 * http://www.holliot.com/
 * All rights reserved.
 *
 *
 * 
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup cc26xx-sensortag/cc2650
 * @{
 *
 * \defgroup common-cc26xx-peripherals CC13xx/CC26xx peripheral driver pool
 *
 * Drivers for peripherals present on more than one CC13xx/CC26xx board. 
 *
 * @{
 *
 * \defgroup sensortag-cc26xx-spi SensorTag/cc2650 SPI functions
 * @{
 *
 * \file
 * Header file for the Sensortag/cc2650 SPI Driver
 * 
 * \NOTE
 * The functions defined in this file are for SPI Slave.
 * SPI master operation are access
 */
/*---------------------------------------------------------------------------*/
#ifndef ___SPI_DRIVER_HEADER__
#define ___SPI_DRIVER_HEADER__
/*---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "spi_api.h"

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

#define	SPI_TX_DEPTH				8U
#define	SPI_RX_DEPTH				8U

#define	CC26XX_SSI_INTERRUPT_ALL		(SSI_RXFF | SSI_RXTO | SSI_TXFF | SSI_RXOR)
#define	CC26XX_SSI_RX_INTERRUPT_TRIGGERS	(SSI_RXFF | SSI_RXTO | SSI_RXOR)

/* spi data width is 8bits, so max data length is 255 */

#define	R_BUFFER_SIZE			128U

#ifdef	SPI_PACKETBUF_CONF_SIZE
#undef	R_BUFFER_SIZE
#if	(SPI_PACKETBUF_CONF_SIZE > 256)
#define	R_BUFFER_SIZE			256U
#else
#define	R_BUFFER_SIZE			SPI_PACKETBUF_CONF_SIZE
#endif /* end if */
#endif /* end ifdef */


#define SPI_DATA_WIDTH_8_BITS		8
#define SPI_DATA_WIDTH_16_BITS		16
/*---------------------------------------------------------------------------*/

/**
 */
void
ssi_rx_handler(void);

/**
 */
void
ssi_int_enable(void);

/**
 */
void
board_spi_open_master(uint32_t bit_rate, uint32_t clk_pin);

/**
 */
void
board_spi_open_slave(uint32_t bit_rate, uint32_t clk_pin);


#endif /* ___SPI_DRIVER_HEADER__ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
