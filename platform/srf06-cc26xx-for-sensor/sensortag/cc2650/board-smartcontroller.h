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
/** \addtogroup cc26xx-srf-tag
 * @{
 *
 * \defgroup sensortag-cc26xx-specific CC2650 Sensortag Peripherals
 *
 * Defines related to the CC2650 Sensortag
 *
 * This file provides connectivity information on LEDs, Buttons, UART and
 * other peripherals
 *
 * This file can be used as the basis to configure other boards using the
 * CC13xx/CC26xx code as their basis.
 *
 * This file is not meant to be modified by the user.
 * @{
 *
 * \file
 * Header file with definitions related to the I/O connections on the TI
 * Sensortag
 *
 * \note   Do not include this file directly. It gets included by contiki-conf
 *         after all relevant directives have been set.
 */
/*---------------------------------------------------------------------------*/
#ifndef BOARD_SMARTCONTROLLER_H_
#define BOARD_SMARTCONTROLLER_H_
/*---------------------------------------------------------------------------*/
#include "ioc.h"
/*---------------------------------------------------------------------------*/
/**
 * \name LED configurations
 *
 * Those values are not meant to be modified by the user
 * @{
 */
/* Some files include leds.h before us, so we need to get rid of defaults in
 * leds.h before we provide correct definitions */
#undef LEDS_GREEN
#undef LEDS_YELLOW
#undef LEDS_RED
#undef LEDS_CONF_ALL

#define LEDS_RED       1
#define LEDS_GREEN     2
#define LEDS_YELLOW    LEDS_GREEN
#define LEDS_ORANGE    LEDS_RED

#define LEDS_CONF_ALL  3

/* Notify various examples that we have LEDs */
#define PLATFORM_HAS_LEDS        1
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name LED IOID mappings
 *
 * Those values are not meant to be modified by the user
 * @{
 */

#define BOARD_IOID_SDA_HP         IOID_UNUSED /**< Interface 1 SDA: MPU */
#define BOARD_IOID_SCL_HP         IOID_UNUSED /**< Interface 1 SCL: MPU */

#define BOARD_IOID_UART_CTS       IOID_UNUSED
#define BOARD_IOID_UART_RTS       IOID_UNUSED
#define BOARD_UART_CTS            (1 << BOARD_IOID_UART_CTS)
#define BOARD_UART_RTS            (1 << BOARD_IOID_UART_RTS)
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Button IOID mapping
 *
 * Those values are not meant to be modified by the user
 * @{
 */
/** @} */

#define BOARD_IOID_DP4_UARTRX		IOID_UNUSED
#define BOARD_IOID_DP5_UARTTX		IOID_UNUSED

#define BOARD_IOID_DP2                IOID_UNUSED
#define BOARD_IOID_DP1                IOID_UNUSED
#define BOARD_IOID_DP0                IOID_UNUSED
#define BOARD_IOID_DP3                IOID_UNUSED
#define BOARD_IOID_DEVPK_ID           IOID_UNUSED
#define BOARD_IOID_DEVPACK_CS         IOID_UNUSED
#define BOARD_DEVPACK_CS              (1 << BOARD_IOID_DEVPACK_CS)

#define BOARD_IOID_FLASH_CS       IOID_UNUSED
#define BOARD_FLASH_CS            (1 << BOARD_IOID_FLASH_CS)
#define BOARD_IOID_SPI_CLK_FLASH  IOID_UNUSED
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Reed Relay IOID mapping
 *
 * Those values are not meant to be modified by the user
 * @{
 */
#define BOARD_IOID_REED_RELAY     IOID_UNUSED
/** @} */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/**
 */

#define BOARD_IOID_KEY_LEFT       IOID_UNUSED
#define BOARD_IOID_KEY_RIGHT      IOID_UNUSED
#define BOARD_KEY_LEFT            (1 << BOARD_IOID_KEY_LEFT)
#define BOARD_KEY_RIGHT           (1 << BOARD_IOID_KEY_RIGHT)
/*******************************************************************************/

#define	BOARD_IOID_DIN_1		IOID_12

#define	BOARD_IOID_RF_CTL1		IOID_1
#define	BOARD_IOID_RF_CTL2		IOID_2
#define	BOARD_IOID_RF_CTL3		IOID_13

#define	BOARD_IOID_DIN_2		IOID_4
#define	BOARD_IOID_DIN_3		IOID_3

#define BOARD_IOID_SDA            IOID_5 /**< Interface 0 SDA: All sensors bar MPU */
#define BOARD_IOID_SCL            IOID_6 /**< Interface 0 SCL: All sensors bar MPU */

#define	BOARD_IOID_DIN_4		    IOID_0
#define	BOARD_IOID_DIN_5		    IOID_7
#define	BOARD_IOID_DIN_6		    IOID_8
#define	BOARD_IOID_DIN_7		    IOID_10
#define	BOARD_IOID_DIN_8		    IOID_11

#define	BOARD_IOID_SPI_FCSN		    IOID_14 
#define BOARD_IOID_TDO		        IOID_16
#define	BOARD_IOID_SPI_UART		    IOID_9
#define BOARD_IOID_SPI_SCLK_TDI		IOID_17  
#define	BOARD_SPI_SCK			   (1 << BOARD_IOID_SPI_SCLK_TDI)

#define BOARD_IOID_UART_RX		    IOID_15
#define BOARD_IOID_UART_TX		    IOID_18
#define BOARD_UART_RX             (1 << BOARD_IOID_UART_RX)
#define BOARD_UART_TX             (1 << BOARD_IOID_UART_TX)

#define	BOARD_IOID_SPI_HC595		IOID_20
#define BOARD_IOID_SPI_MOSI		    IOID_19
#define BOARD_IOID_SPI_MISO		    IOID_21
#define	BOARD_SPI_MOSI			(1 << BOARD_IOID_SPI_MOSI)
#define	BOARD_SPI_MISO			(1 << BOARD_IOID_SPI_MISO)


#define BOARD_IOID_SPI_IRQ		    IOID_23
#define BOARD_IOID_C2V		        IOID_26
#define	BOARD_IOID_MONITOR_12V		IOID_27
#define	BOARD_IOID_MONITOR_5V		IOID_28


/******************************IOID22,IOID24,IOID25,IOID29,IOID30 reserved************************************/
 
 
#define	BOARD_IOID_UNUSED_IOID22		IOID_22
#define	BOARD_IOID_UNUSED_IOID24		IOID_24
#define	BOARD_IOID_UNUSED_IOID25		IOID_25
#define	BOARD_IOID_UNUSED_IOID29		IOID_29
#define	BOARD_IOID_UNUSED_IOID30		IOID_30
 
 
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Button IOID mapping
 *
 * Those values are not meant to be modified by the user
 * @{
 */
/** @} */

/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Reed Relay IOID mapping
 *
 * Those values are not meant to be modified by the user
 * @{
 */
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Buzzer configuration
 * @{
 */
#define BOARD_IOID_BUZZER         IOID_UNUSED /**< Buzzer Pin */
/** @} */
/*---------------------------------------------------------------------------*/
/**
 */

/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \brief MPU IOID mappings
 *
 * Those values are not meant to be modified by the user
 * @{
 */
#define BOARD_IOID_MPU_INT        IOID_UNUSED
#define BOARD_IOID_MPU_POWER      IOID_UNUSED
#define BOARD_MPU_INT             (1 << BOARD_IOID_MPU_INT)
#define BOARD_MPU_POWER           (1 << BOARD_IOID_MPU_POWER)

#define BOARD_IOID_TMP_RDY          IOID_UNUSED

/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Device string used on startup
 * @{
 */
#define BOARD_STRING "TI CC2650 SMARTCONTROLLER"

/** @} */
/*---------------------------------------------------------------------------*/
#endif /* BOARD_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
