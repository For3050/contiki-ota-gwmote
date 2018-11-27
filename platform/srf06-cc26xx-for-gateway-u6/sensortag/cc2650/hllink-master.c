/***
 *
 *
 *
 * */
#include "contiki.h"
#include "contiki-lib.h"	
//#include "lib/ringbuf.h"
//#include "list.h"
//#include "memb.h"
#include "spiToUart_driver.h"
#include "hllink.h"

#include <stdio.h>
#include <string.h>
#define		SBUF_INVALID				0x04

#define		RS485_MASTER		0
#define		RS485_SLAVE		1
#define		W_BUF_SIZE			128
#define		MAX_SLAVE_COUNT		128
#define		MAX_SIZE		128
static uint8_t slave_arr[MAX_SLAVE_COUNT]={0x00,};

/********************************************************************************************/
#define BUF_SIZE		64
static uint8_t rs485_sbuf[BUF_SIZE] = {0x00,};
static uint8_t* read_ptr = NULL;

/********************************************************************************************/
void rs485_sbuf_reset(void)
{
		memset(rs485_sbuf, 0x00, BUF_SIZE);
		rs485_sbuf[1] = SBUF_INVALID;
}
/********************************************************************************************/
extern bool rs485_master_send(uint8_t dest_addr, uint8_t *data, uint8_t datalen)
{
	if((dest_addr < 0) ||(dest_addr > 128) || (datalen > 64) || (datalen < 0))
		return 0;
	
	memset(rs485_sbuf, 0x00, BUF_SIZE);	
	if(ACK_ASK_WITH_DATA_TOMST == read_ptr[2])
		{
			printf("To be defined!\n");
		}
	else
		{
			rs485_sbuf[0] = dest_addr;
			rs485_sbuf[1] = FUNC_CODE_WRITE;
			rs485_sbuf[2] = datalen;
			memcpy(rs485_sbuf+3, data, datalen);
		}

	return 1;
}


/********************************************************************************************/
extern void rs485_master_recv(uint8_t *read_ptr )
{
	printf("Master recieved data from slave:%d!\n", read_ptr[1]);
	for(int i = 0; i < read_ptr[0]; i++)
	{
		printf("%x, ", read_ptr[i + 1]);
	}
	printf("\n");
}

/********************************************************************************************/



/********************************************************************************************/


#define	HANDSHAKE_WITH_SLAVE_I(i)	do{\	
						uint8_t w_buf[2] = { ##i, FUNC_CODE_HANDSHAKE};\
						rs485_write_data(w_buf, 2);\
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
			for(i = 0; i < my_local_master.total_slave_count; i++)				\
			{										\
				if(slave_arr[i] == slave_addr_i)					\
				{									\
					slave_arr[i] = 0xFF;						\
					break;								\
				}									\
			}										\
			printf("slave %d timeout.\n", slave_addr_i);					\
			continue;/* end the current loop, goto the next loop */				\
		}											\
		read_ptr = (uint8_t*)data;								\
		if((read_ptr[1] == slave_addr_i) &&(read_ptr[2] == ACK_HANDSHAKE) )			\
		{											\
			/*add the new slave at the first location of the table*/			\
			printf("slave %d handshake success.\n", slave_addr_i);				\
			for( i = 0; i < MAX_SLAVE_COUNT; i++)			 			\
			{										\
				if(slave_arr[i] == 0xFF)	/* empty location */				\
				{									\
					slave_arr[i] = slave_addr_i;					\
					my_local_master.total_slave_count++;				\
					break;								\
				}									\
			}										\
			if(i == MAX_SLAVE_COUNT)							\
			{										\
				printf("warning:no empty location.\n");					\
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
struct rs485_master my_local_master = {0, 0, 1, 0};

/* each slave addr is particular (range: 1~127) */
typedef struct rs485_slave
{
	uint8_t slave_addr;
	struct rs485_slave *next;
}rs485_slave;

//LIST(slave_table);
//MEMB(slaves_mem, struct rs485_slave, MAX_SLAVE_COUNT);

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



/****************************************************************************/






PROCESS_THREAD(rs485_master_process, ev, data)
{

	static uint8_t slave_addr_i=1;

	static struct etimer et;
	static uint8_t i;
	static uint8_t slv1_addr = 0x00;
	static uint8_t slv2_addr = 0x00;
//	static struct rs485_slave *e = NULL;

	PROCESS_BEGIN();
	memset(slave_arr, 0xFF, MAX_SLAVE_COUNT);
//	list_init(slave_table);
//	memb_init(&slaves_mem);

	// 1, master phy init
	
	// 2, fisrtly, use the max slave count loop for handshaking, update slave tablfor(slave_addr_i = 1; slave_addr_i <= MAX_SLAVE_COUNT; slave_count_i++)
	HANDSHAKE_WITH_SLAVES();

	// 3, set ctimer
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
		for(i = 0; i < my_local_master.total_slave_count; i++)
		{
				// 1, request slave e
				uint8_t req_buf[2]={slave_arr[i], FUNC_CODE_ASK};				
				rs485_write_data(req_buf, 2);

				// 2, wait for ack
				ctimer_set(&timeout_timer, CLOCK_SECOND >> 2, timeout_callback, &slave_arr[i]);	
				PROCESS_WAIT_EVENT_UNTIL(ev == rs485_recv_event);					
				ctimer_stop(&timeout_timer);								
				printf("ask slave %d \n", slave_arr[i]);
				if(my_local_master.slave_timeout)							
				{											
					my_local_master.slave_timeout = 0;						
					printf("MASTER:ask slave %d timeout.\n", slave_arr[i]);					
					continue;/* end the current loop, goto the next loop */				
				}											
				
				// 3,  handle slave request
				read_ptr = (uint8_t*)data;								
				if( read_ptr[1] == slave_arr[i] )			
				{											
					printf("/**************MASTER START******************/\n");
					if(ACK_ASK_WITH_NOTHING == read_ptr[2])
					{
						printf("MASTER:slave %d has nothing to do.\n",slave_arr[i]);
						printf("\n");
						printf("\n");
					}
					else if(ACK_ASK_WITH_DATA_TOMST == read_ptr[2])								
					{					
						uint8_t j;
						
						printf("/********************************/\n");
						printf("MASTER:recved slave %d data: ", slave_arr[i]);
						for(j = 0; j < read_ptr[3]; j++)
							printf("%x,", read_ptr[4+j]);
						printf("/********************************/\n");
					}
					else if(ACK_ASK_WITH_DATA_TOSLV == read_ptr[2])								
					{
						printf("/********************************/\n");
						printf("MASTER:slave %d want to send data to slave %d.\n", slave_arr[i], read_ptr[3]);
						
						/* 1, first, save slave1_addr */
						slv1_addr = read_ptr[1];
						slv2_addr = read_ptr[3];
						
						/* 2, master format data from slave 1,  transfer to slave 2, and set ctimer */
						uint8_t w_buf[W_BUF_SIZE] = {0x00, };
						w_buf[0] = slv2_addr;
						w_buf[1] = FUNC_CODE_WRITE;
						w_buf[2] = read_ptr[0] - 3;
						memcpy(w_buf + 3, read_ptr + 4, read_ptr[0] - 3);
						
						/* transfer to slave 2 */
						rs485_write_data(w_buf, w_buf[2] + 3);
						
						/* set ctimer  */
						ctimer_set(&timeout_timer, CLOCK_SECOND >> 2, timeout_callback, NULL);	
						PROCESS_WAIT_EVENT_UNTIL(ev == rs485_recv_event);					
						ctimer_stop(&timeout_timer);								
						printf("MASTER: data transfered to slave_2: %d \n", slv2_addr);

						/* 3, wait for ack from slave 2: timed out process   */
						if(my_local_master.slave_timeout)							
						{											
							my_local_master.slave_timeout = 0;						
							printf("MASTER: transfer data to slave_2 %d timedout.\n", w_buf[0]);		// w_buf[0] unstable ???????			

							/*  timed out process   */
							w_buf[0] = slv1_addr;
							w_buf[1] = FUNC_CODE_WRITE;
							w_buf[2] = 1;
							w_buf[3] = 0xff;	
							
							rs485_write_data(w_buf, w_buf[2] + 3);
							continue;
						}							
						
						/* 3, wait for ack: receive ack from slave 2   */
						read_ptr = (uint8_t *)data;
						
						/* 4, master format data from slave 2, and transfer to slave 1 without ack  */
						if((read_ptr[1] == slv2_addr) && (read_ptr[2] == ACK_WRITE))
							{
								printf("MASTER: slave_2: %d acked.\n", read_ptr[1]);
								for(int i = 0; i < read_ptr[0]; i++)
									{
										printf("%x, ", read_ptr[i + 1]);
									}
								printf("\n");
								printf("\n");
						
								w_buf[0] = slv1_addr;
								w_buf[1] = FUNC_CODE_WRITE;
								w_buf[2] = read_ptr[3];
								memcpy(w_buf + 3, read_ptr + 4, w_buf[2]);
								
								/* transfer to slave 1 */
								rs485_write_data(w_buf, w_buf[2] + 3);
								printf("MASTER: transfered to slave 1:%d.\n", slv1_addr);
								
								/* set ctimer  */
								ctimer_set(&timeout_timer, CLOCK_SECOND >> 2, timeout_callback, NULL);	
								PROCESS_WAIT_EVENT_UNTIL(ev == rs485_recv_event);					
								ctimer_stop(&timeout_timer);	

								if(my_local_master.slave_timeout)							
								{											
									my_local_master.slave_timeout = 0;
									printf("MASTER: slave-slave completed without slave 1 ack \n");
									continue;
								}
								read_ptr = (uint8_t *)data;
								for(int i = 0; i < read_ptr[0]; i++)
									{
										printf("%x, ", read_ptr[i + 1]);
									}
								printf("\n");
								printf("MASTER: slave-slave completed with slave 1 acked ! \n");						
			
							}
						printf("/********************************/\n");

					}
					else if(ACK_WRITE == read_ptr[2])
						{
							printf("/********************************/\n");
							printf("MASTER:WARNNING !!!!!!recieved ack_write");
							printf("/********************************/\n");
						}
					if(rs485_sbuf[1] == FUNC_CODE_WRITE)
						{
							uint8_t w_buf[W_BUF_SIZE] = {0x00, };
							
							memcpy(w_buf, rs485_sbuf,  rs485_sbuf[2] + 3);
							slv2_addr  = rs485_sbuf[0];
							
							rs485_write_data(w_buf, w_buf[2] + 3);
							rs485_sbuf_reset();

							/* set ctimer  */
							ctimer_set(&timeout_timer, CLOCK_SECOND >> 2, timeout_callback, NULL);	
							PROCESS_WAIT_EVENT_UNTIL(ev == rs485_recv_event);					
							ctimer_stop(&timeout_timer);								
							printf("MASTER: data transfered to slave_2: %d \n", slv2_addr);

							if(my_local_master.slave_timeout)							
							{											
								my_local_master.slave_timeout = 0;						
								printf("MASTER: transfer data to slave_2 %d timedout.\n", w_buf[0]);		// w_buf[0] unstable ???????			

								/*  timed out process   */
								printf("To be defined!\n");

								continue;
							}		

							/* 3, wait for ack  */
							read_ptr = (uint8_t *)data;
							
							/* 4,  */
							if((read_ptr[1] == slv2_addr) && (read_ptr[2] == ACK_WRITE))
								{
									printf("Master recved write_ack!\n");
									printf("To be defined!\n");
								}

							
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


