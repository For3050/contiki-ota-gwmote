/***
 *
 *
 *
 * */
#include "contiki.h"
#include "contiki-lib.h"	
#include "lib/ringbuf.h"
#include "list.h"
#include "memb.h"
#include "spiToUart_driver.h"
#include "hllink.h"

#include <stdio.h>

#define		RS485_MASTER		0
#define		RS485_SLAVE		1

#define		MAX_SLAVE_COUNT		4


#define	HANDSHAKE_WITH_SLAVE_I(i)	do{									\
						uint8_t w_buf[2] = { ##i, FUNC_CODE_HANDSHAKE};			\
						rs485_write_data(w_buf, 2);					\
					}while(0)



#define	HANDSHAKE_WITH_SLAVES()		do{		\
	for(slave_addr_i = 1; slave_addr_i <= MAX_SLAVE_COUNT; slave_addr_i++)				\
	{												\
													\
		uint8_t w_buf[2] = { slave_addr_i, FUNC_CODE_HANDSHAKE};				\
		rs485_write_data(w_buf, 2);								\
													\
		ctimer_set(&timeout_timer, CLOCK_SECOND >> 2, timeout_callback, &slave_addr_i);		\
		PROCESS_WAIT_EVENT_UNTIL(ev == rs485_recv_event);					\
		ctimer_stop(&timeout_timer);								\
		if(my_local_master.slave_timeout)							\
		{											\
			my_local_master.slave_timeout = 0;						\
			/* remove no ack slave */							\
			for(e = list_head(slave_table); e != NULL; e = e->next)				\
			{										\
				if(e->slave_addr == slave_addr_i)					\
					list_remove(slave_table, e);					\
			}										\
			printf("slave %d timeout.\n", slave_addr_i);					\
			continue;/* end the current loop, goto the next loop */				\
		}											\
		read_ptr = (uint8_t*)data;								\
		if((read_ptr[1] == slave_addr_i) &&(read_ptr[2] == ACK_HANDSHAKE) )			\
		{											\
			/*add the new slave at the first location of the table*/			\
			printf("slave %d handshake success.\n", slave_addr_i);				\
			for(e = list_head(slave_table); e != NULL; e = e->next)				\
			{										\
				if(e->slave_addr == slave_addr_i)					\
					break;								\
			}										\
			if(e == NULL)									\
			{										\
				e = memb_alloc(&slaves_mem);						\
				if(e != NULL)								\
				{									\
					e->slave_addr = slave_addr_i;							\
					printf("slave1: %d,slave2:%d\n", e->slave_addr, slave_addr_i);		\
					list_push(slave_table, e);/* add the new slave at the fisrt of slave table */	\
					e = list_pop(slave_table);							\
					printf("list head slave:%d\n", e->slave_addr );		\
				}									\
			}										\
		}											\
	}												\
}while(0)												


#if 0

#endif								

//#define	RXBUF_SIZE	128
//static struct ringbuf master_rxbuf;
//static uint8_t rxbuf_data[RXBUF_SIZE];

PROCESS(rs485_master_process,"rs485 master link layer process");


typedef	struct rs485_master
{
	uint8_t is_tx;
	uint8_t total_slave_count;
	uint8_t max_retx;
	uint8_t slave_timeout;
}rs485_master;
struct rs485_master my_local_master = {0, MAX_SLAVE_COUNT, 1, 0};

/* each slave addr is particular (range: 1~127) */
typedef struct rs485_slave
{
	uint8_t slave_addr;
	struct rs485_slave *next;
}rs485_slave;

LIST(slave_table);
MEMB(slaves_mem, struct rs485_slave, MAX_SLAVE_COUNT);

static struct ctimer timeout_timer;
void timeout_callback(void* arg)
{
	my_local_master.slave_timeout = 1;
	process_post(&rs485_master_process, rs485_recv_event, NULL);
}

static uint8_t loop_for_all = 0;
static struct ctimer loop_timer; 
void loop_for_handshake(void* arg)
{
	loop_for_all = 1;
}





PROCESS_THREAD(rs485_master_process, ev, data)
{

	static uint8_t slave_addr_i=1;
	static uint8_t* read_ptr;
	static struct etimer et;
	static struct rs485_slave *e = NULL;

	PROCESS_BEGIN();

	list_init(slave_table);
	memb_init(&slaves_mem);

	//1.master phy init
	
	//2.fisrtly, use the max slave count loop for handshaking, update slave tablfor(slave_addr_i = 1; slave_addr_i <= MAX_SLAVE_COUNT; slave_count_i++)
	HANDSHAKE_WITH_SLAVES();

	//3. set ctimer
	ctimer_set(&loop_timer, CLOCK_SECOND*30, loop_for_handshake, NULL);

	while(1)
	{
		etimer_set(&et, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL( etimer_expired(&et) );

		if(loop_for_all)
		{
			loop_for_all = 0;
			printf("start a new handshake procedure.\n");

			HANDSHAKE_WITH_SLAVES();

			ctimer_reset(&loop_timer);

		}
		//because the write operation is voluntary, so this process loop for requsting all slave
		for(e = list_head(slave_table); e != NULL; e = e->next)
		{
			//1. request slave e
			uint8_t req_buf[2] = {e->slave_addr, FUNC_CODE_ASK};	
			rs485_write_data(req_buf, 2);

			//2. wait for ack
			ctimer_set(&timeout_timer, CLOCK_SECOND >> 2, timeout_callback, &(e->slave_addr));	
			PROCESS_WAIT_EVENT_UNTIL(ev == rs485_recv_event);					
			ctimer_stop(&timeout_timer);								
			printf("ask slave %d \n", e->slave_addr);
			if(my_local_master.slave_timeout)							
			{											
				my_local_master.slave_timeout = 0;						
				printf("ask slave %d timeout.\n", e->slave_addr);					
				continue;/* end the current loop, goto the next loop */				
			}											
			
			//3. handle slave request
			read_ptr = (uint8_t*)data;								
			if(read_ptr[1] == e->slave_addr)			
			{											
				if(ACK_ASK_WITH_NOTHING == read_ptr[2])
				{
					printf("slave %d has nothing to do.\n",e->slave_addr);
				}
				else if(ACK_ASK_WITH_DATA_TOMST == read_ptr[2])
				{
					uint8_t i;
					printf("recv slave %d data: ", e->slave_addr);
					for(i = 0; i < read_ptr[3]; i++)
						printf("0x%x, ", read_ptr[4+i]);
					printf("\n");

				}
				else if(ACK_ASK_WITH_DATA_TOSLV == read_ptr[2])
				{
					printf("slave %d want to send data to slave %d.\n", e->slave_addr, read_ptr[3]);
				}
			}		

			//
			//
		}

	}
	PROCESS_END();

}

void rs485_link_init(void)
{
	//ringbuf_init(&master_rxbuf, rxbuf_data, sizeof(rxbuf_data));
	rs485_init();
	process_start(&rs485_master_process, NULL);
}


