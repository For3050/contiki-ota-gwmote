
#include "contiki.h"
#include "ti-lib.h"
#include "board.h"
#include "board-spi.h"
#include "spiToUart_driver.h"
#include "gpio-interrupt.h"
//#include <stdint.h>
#include <stdio.h>
//#include <stdbool.h>
#include "lib/ringbuf.h"
#include "sys/energest.h"

/* NOTE: It is necessary to check the value of LCR regsiter before w/r registers
 *
 *
 */
PROCESS(rs485_phy_process, "rs485 phy layer process");
process_event_t rs485_recv_event;
//static 

#define		SC16IS762_RX_INTERRUPT_TRIGGERS		(SC16IS762_INT_RX_TIMEOUT | SC16IS762_INT_RX_READY)
#define		READ_BUF_SIZE		128
/***************  read/write registers by spi, default channel:A  ****************/
void write_reg8_by_spi(const uint8_t reg_addr, const uint8_t channel,const uint8_t value)
{
	const uint8_t wBuf[2] = {WRITE_TO_UART | (reg_addr << 3) | channel, value};
	ti_lib_gpio_clear_dio(BOARD_IOID_SPI_UART);
	board_spi_write(wBuf,sizeof(wBuf)/sizeof(uint8_t));
	ti_lib_gpio_set_dio(BOARD_IOID_SPI_UART);
}

uint8_t read_reg8_by_spi(uint8_t reg_addr,uint8_t channel)
{
	uint8_t rVal;
	const uint8_t reg_addr_byte = READ_FROM_UART | (reg_addr << 3) | channel;
	ti_lib_gpio_clear_dio(BOARD_IOID_SPI_UART);
	board_spi_write(&reg_addr_byte, sizeof(reg_addr_byte));
	board_spi_read(&rVal, 1);
	ti_lib_gpio_set_dio(BOARD_IOID_SPI_UART);
	return rVal;
	
}
/***************  read/write registers by spi, default channel:A  ****************/


void rs485_write_data(uint8_t* w_buf, uint8_t len)
{
	uint8_t first_bytes[2] ={ WRITE_TO_UART | (THR_ADDR << 3) | SEL_CHAN_B , len};

	ti_lib_gpio_clear_dio(BOARD_IOID_SPI_UART);

	board_spi_write(first_bytes, 2);

	board_spi_write(w_buf, len);

	ti_lib_gpio_set_dio(BOARD_IOID_SPI_UART);

}

/* this function only serviced RHR and THR interrupt
 * reserved to do: create a new thread to replace of calling spi_read_data
 * and rs485_write_data
 */

//interrupt handler
static uint8_t readBuf[READ_BUF_SIZE] = {0x00};
static uint8_t	read_data_len = 0;
static void
spi_interrupt_handler(uint8_t ioid)
{

	
	uint8_t intr_ident_reg = 0;
	
	ENERGEST_ON(ENERGEST_TYPE_IRQ);

	intr_ident_reg = read_reg8_by_spi( IIR_ADDR,SEL_CHAN_B );

	ti_lib_ioc_int_clear(ioid);

	if( (intr_ident_reg & 0x0F) == 0x06) /* receive line status error */
	{
		printf("*****data error occured.*****\n");
		return;
	}
	/* if this was a RX FIFO or an RX time-out interrupt, read all bytes available in the RX FIFO */
	if( (intr_ident_reg & SC16IS762_RX_INTERRUPT_TRIGGERS) != 0 ) /* RHR interrupt */
	{
		while( (read_reg8_by_spi(LSR_ADDR, SEL_CHAN_B) & 0x01) || ( (read_reg8_by_spi( IIR_ADDR,SEL_CHAN_B ) & 0x0F) == 0x0c)  )
		{
			readBuf[read_data_len++] = read_reg8_by_spi( RHR_ADDR, SEL_CHAN_B );
			if(read_data_len == readBuf[0] + 1)
			{
				process_post(PROCESS_BROADCAST, rs485_recv_event, readBuf);
			//	process_poll(&rs485_phy_process);
				read_data_len = 0;
			}
		}
	}


	ENERGEST_OFF(ENERGEST_TYPE_IRQ);

}
/*
 * HW control flow disabled, special character detect disabled, 
 * enhanced functions enabled, Combinations of software flow control can be selected 0000(binary)
 * --> EFR = 0x10
*/


//Register isr interrupts
static void register_isr_interrupts(void)
{
	ti_lib_ioc_int_disable(BOARD_IOID_SPI_IRQ);
	ti_lib_ioc_int_clear(BOARD_IOID_SPI_IRQ);
	ti_lib_gpio_clear_event_dio(BOARD_IOID_SPI_IRQ);
	ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_IRQ);

	ti_lib_gpio_set_dio(BOARD_IOID_SPI_IRQ);/* put the pin original status high, the interrupt signal is low-active. */

	gpio_interrupt_register_handler(BOARD_IOID_SPI_IRQ, spi_interrupt_handler);
	ti_lib_ioc_io_int_set(BOARD_IOID_SPI_IRQ,IOC_INT_DISABLE,IOC_FALLING_EDGE);
	//ti_lib_ioc_int_register(spi_interrupt_handler);
	ti_lib_ioc_int_enable(BOARD_IOID_SPI_IRQ);
	//ti_lib_int_enable(INT_AON_GPIO_EDGE);

}

//channel = SEL_CHAN_B
int sc16is762_CHAN_B_init(void)
{
	uint16_t	divisor;
	uint8_t		reg_value;


	ti_lib_gpio_set_dio(BOARD_IOID_SPI_FCSN);
	ti_lib_gpio_set_dio(BOARD_IOID_SPI_FSS_EXT);
	ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_FCSN);
	ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_FSS_EXT);

	ti_lib_gpio_set_dio(BOARD_IOID_SPI_IRQ);
	ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_SPI_IRQ);

	board_spi_open(4000000, BOARD_IOID_SPI_SCLK_TDI);
	ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_SPI_UART);
	ti_lib_gpio_set_dio(BOARD_IOID_SPI_UART);
	clock_delay_usec(10);

	write_reg8_by_spi( IER_ADDR, SEL_CHAN_B,0x00 );

#if	0
	printf("400KH\n");
	write_reg8_by_spi( LCR_ADDR,SEL_CHAN_B, 0x8B );
//	divisor = (XTAL1_INPUT_CLOCK/4) / (38400 * 16);
	divisor = XTAL1_INPUT_CLOCK / (460800 * 16);
	printf("divisor=%d\n",divisor);
	write_reg8_by_spi( DLL_ADDR, SEL_CHAN_B,(divisor & 0xFF) );
	write_reg8_by_spi( DLH_ADDR, SEL_CHAN_B, (divisor >> 8) );
#endif
	/* Enhanced Features Registers are only accessible when LCR = 0xBF */
	write_reg8_by_spi( LCR_ADDR, SEL_CHAN_B,0xBF );

	/* enables the enhanced function IER[7:4], FCR[5:4], and MCR[7:5] so that they can be modified */
	write_reg8_by_spi( EFR_ADDR,SEL_CHAN_B, 0x10 );

	write_reg8_by_spi( LCR_ADDR, SEL_CHAN_B,0x3B );


	write_reg8_by_spi( MCR_ADDR, SEL_CHAN_B,0x04 );
	reg_value = read_reg8_by_spi(MCR_ADDR,SEL_CHAN_B);
	printf("MCR=0x%x,\n", reg_value);

	write_reg8_by_spi( TLR_ADDR, SEL_CHAN_B,0xee );
	write_reg8_by_spi( TCR_ADDR,SEL_CHAN_B, 0x4F);

#if	1
	write_reg8_by_spi( LCR_ADDR,SEL_CHAN_B, 0x8B );
//	divisor = (XTAL1_INPUT_CLOCK/4) / (38400 * 16);
	divisor = XTAL1_INPUT_CLOCK / (307200 * 16);
	printf("divisor=%d\n",divisor);
	write_reg8_by_spi( DLL_ADDR, SEL_CHAN_B,(divisor & 0xFF) );
	write_reg8_by_spi( DLH_ADDR, SEL_CHAN_B, (divisor >> 8) );

	write_reg8_by_spi( LCR_ADDR, SEL_CHAN_B,0x3B );
#endif


	write_reg8_by_spi(EFCR_ADDR, SEL_CHAN_B, 0x31);

	/* when TLR is used for RX trigger level control, FCR[7:6] should be left at the default state '00' */
	write_reg8_by_spi(FCR_ADDR,SEL_CHAN_B,0x07);


	clock_delay_usec(100);
	/* general registers are ts, parity control, parity type */


	/* enable RHR and receive line status interrupts */

	write_reg8_by_spi( IER_ADDR,SEL_CHAN_B, 0x05 );

	register_isr_interrupts();

	return 0;
	
	
}

PROCESS_NAME(rs485_master_process);
#if	1
PROCESS_THREAD(rs485_phy_process, ev, data)
{
	static int i = 0;
	PROCESS_BEGIN();

	printf("%s start.\n", rs485_phy_process.name);

	while(1)
	{
		PROCESS_YIELD();
		if(ev == PROCESS_EVENT_POLL)
		{
		//	process_post_synch(&rs485_master_process, rs485_recv_event, &read_data_len);
			process_post(PROCESS_BROADCAST, rs485_recv_event, readBuf);
#if	0
			/* Wait until all processes have handled the serial line event */
			if(PROCESS_ERR_OK ==
					process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
				PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
			}

			read_data_len = 0;
#endif
#if	0
			for(i = 0; i < read_data_len; i++)
			{
				printf("0x%x, ",readBuf[i]);
			}
			read_data_len = 0;
			printf("\n");
#endif
		}
	}

	PROCESS_END();
}
#endif

void rs485_init(void)
{

	sc16is762_CHAN_B_init();
	rs485_recv_event = process_alloc_event();
	process_start(&rs485_phy_process, NULL);
}


 // Set Direction on UART GPIO Port pins GPIO0 to GPIO7
// 0=input   1=Output  
//void Set_GPIO_Dir(uint8_t channel,uint8_t logic)                                                                  
//{ 
      
//	write_reg8_by_spi (LCR_ADDR,read_reg8_by_spi(LCR_ADDR,channel)&0x7F, channel);
//	write_reg8_by_spi (IOControl_ADDR, 0x03,channel); // Set the IOControl Register to GPIO Control 
//	write_reg8_by_spi (IODir_ADDR,logic,channel); // output the control bits to the IO Direction Register 
//}

 //Read UART GPIO Port 
//uint8_t Read_GPIO(uint8_t channel)                                                                  
//{ 
//	uint8_t data = 0x00;  
//	write_reg8_by_spi (LCR_ADDR,read_reg8_by_spi(LCR_ADDR,channel)&0x7F, channel);
//	data=read_reg8_by_spi(IOState_ADDR,channel);
//	return data;
//}

//Load UART GPIO Port
//void Write_GPIO(uint8_t channel,uint8_t data)                                                                  
//{ 
//	write_reg8_by_spi(LCR_ADDR,read_reg8_by_spi(LCR_ADDR,channel)&0x7F, channel);        
//	write_reg8_by_spi(IOState_ADDR,data, channel); // set GPIO Output pins state 0-7 
//} 

