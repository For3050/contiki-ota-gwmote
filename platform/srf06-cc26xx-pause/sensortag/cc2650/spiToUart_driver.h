#ifndef		__SPI_TO_UART_HEADER__
#define		__SPI_TO_UART_HEADER__


/*
 * This header file is for SC16IS752/SC16IS762(Dual UART with I2C-bus/SPI interface) 
 * internal registers and API.
 * Author: wuwei
 * Date: 2017/11/13
 */
/*---------------------------------------------------------------------------*/
/*
 * Header file for the SC16IS752_SC16IS762 Driver
 */
/*---------------------------------------------------------------------------*/



/*------------------------ General register set -----------------------------*/

#define RHR_ADDR				0x00 /** Receive Holding Register */
#define THR_ADDR				0x00 /** Transmit Holding Register */
#define IER_ADDR				0x01 /** Interrupt Enable Register */
#define FCR_ADDR				0x02 /** FIFO Control Register */

#define IIR_ADDR				0x02 /** Interrupt Identification Register */
#define LCR_ADDR				0x03 /** Line Control Register */
#define MCR_ADDR				0x04 /** Modem Control Register */
#define LSR_ADDR				0x05 /** Line Status Register */
#define MSR_ADDR				0x06 /** Modem Status Register */
#define SPR_ADDR				0x07 /** Scratchpad Register */

#define TCR_ADDR				0x06 /** Transmission Control Register */
#define TLR_ADDR				0x07 /** Trigger Level Register */
#define TXLVL_ADDR				0x08 /** Transmitter FIFO Level register */
#define RXLVL_ADDR				0x09 /** Receiver FIFO Level register */
#define IODir_ADDR				0x0A /** Programmable I/O pins Direction register */
#define IOState_ADDR				0x0B /** Programmable I/O pins State register */

#define IOIntEna_ADDR				0x0C /** I/O Interrupt Enable register */
#define IOControl_ADDR				0x0E /** I/O Control register */
#define EFCR_ADDR				0x0F /** Extra Features Control Register */

/*------------------------ General register set -----------------------------*/



/*------------------------ Special register set -----------------------------*/
#define DLL_ADDR				0x00 /** Divisor Latch LSB register */
#define DLH_ADDR				0x01 /** Divisor Latch MSB register */
/*------------------------ Special register set -----------------------------*/


/*------------------------ Enhanced register set -----------------------------*/
#define EFR_ADDR				0x02 /** Enhanced Features Register */
#define XON1_ADDR				0x04 /** Xon1 word register */
#define XON2_ADDR				0x05 /** Xon2 word register */
#define XOFF1_ADDR				0x06 /** Xoff1 word register */
#define XOFF2_ADDR				0x07 /** Xoff2 word register */
/*------------------------ Enhanced register set -----------------------------*/


/*------------------------ Register address bits -----------------------------*/
#define WRITE_TO_UART				0x00
#define READ_FROM_UART				0x80

#define SEL_CHAN_A				0x00
#define SEL_CHAN_B				0x02
/*------------------------ Register address bits -----------------------------*/


#define XTAL1_INPUT_CLOCK			14745600


/*-------------------------- interrupts ----------------------------*/
#define		SC16IS762_INT_RX_ERROR			(3 << 1)
#define		SC16IS762_INT_RX_TIMEOUT		(3 << 2)
#define		SC16IS762_INT_RX_READY			(1 << 2)



int sc16is762_CHAN_A_init();
int sc16is762_CHAN_B_init();
void write_reg8_by_spi(const uint8_t reg_addr, const uint8_t channel,const uint8_t value);
uint8_t read_reg8_by_spi(uint8_t reg_addr,uint8_t channel);
void Set_GPIO_Dir(uint8_t channel,uint8_t logic); 
void rs485_write_data(uint8_t* w_buf, uint8_t len);
void rs485_init(void);
uint8_t Read_GPIO(uint8_t channel);                                                                  
void Write_GPIO(uint8_t channel,uint8_t data);

extern process_event_t rs485_recv_event;

#endif  /* end of header file */

