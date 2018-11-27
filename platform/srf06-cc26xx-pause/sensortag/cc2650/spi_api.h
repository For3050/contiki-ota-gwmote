/*
 * http://www.holliot.com/
 * All rights reserved.
 * author: jerry_wu(wuwei@holliot.com)
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
#ifndef ___SPI_API_HEADER__
#define ___SPI_API_HEADER__
/*---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

extern process_event_t spi_recv_event;
/*---------------------------------------------------------------------------*/
/**
 * \brief Initialize the SPI interface
 * \param: NULL 
 * Default configuration:
 *			1. bit_rate = 4000000Hz
 *			2. identification: slave
 *			3. mode: SSI_FRF_MOTO_MODE_0  
 *			4. clk_pin  = BOARD_IOID_SPI_SCK
 * \return none
 *
 * This function will make sure the peripheral is powered, clocked and
 * initialised. A chain of calls to spi_read(), spi_write() and
 * spi_flush() must be preceded by a call to this function. It is
 * recommended to call spi_close() after such chain of calls.
 */
void
spi_init(void);

/**
 * \brief Read from an SPI device
 * \param buf The buffer to store data
 * \return data length when successful.
 *
 * Calls to this function must be preceded by a call to spi_init(). It is
 * recommended to call spi_close() at the end of an operation.
 */
uint8_t
spi_read(uint8_t *buf);

/**
 * \brief Write to an SPI device
 * \param buf The buffer with the data to write
 * \param length The number of bytes to write
 * \return True when successful.
 *
 * Calls to this function must be preceded by a call to spi_init(). It is
 * recommended to call spi_close() at the end of an operation.
 */
bool
spi_write(const uint8_t *buf, uint8_t len);
/*---------------------------------------------------------------------------*/

/**
 * \brief Close the SPI interface
 * \return True when successful.
 *
 * This function will stop clocks to the SSI module and will set MISO, MOSI
 * and CLK to a low leakage state. It is recommended to call this function
 * after a chain of calls to spi_read() and spi_write()
 */
void spi_close(void);

/**
 * \brief Clear data from the SPI interface
 * \return none
 */
void spi_flush(void);


#endif /* ___SPI_API_HEADER__ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
