#include "contiki.h"
#include "contiki-lib.h"
#include "spiToUart_driver.h"
#include "hllink.h"

#include <stdio.h>
#include <string.h>
PROCESS(rs485_slave_process, "rs485 link layer slave process");
#define MAX_SIZE 128
static uint8_t my_slave_addr = 0x00;

#define	DO_ACK_HANDSHAKE()	do{	\
			uint8_t w_buf[2] = {my_slave_addr, ACK_HANDSHAKE};	\
			rs485_write_data(w_buf, 2);		\
			}while(0)

// 1,  rs 485 phy init
// 2, wait for handshake
// 3, ack handshake with my local addr
// 4, tell the app handshake result: success or fail
// 5,  if handshake success, app can exchange data with master
// 6, recv process wait
// 7,  send data api called
/****************************************************************************/
#define BUF_SIZE		64
#define MASTER	0
static uint8_t rs485_sbuf[BUF_SIZE] = {0x00,};
static uint8_t slave_is_txing = 0;
//static uint8_t rbuf_ready = 0;
static uint8_t* recv_ptr = NULL;





/****************************************************************************/

void rs485_sbuf_reset(void)
{
		memset(rs485_sbuf, 0x00, BUF_SIZE);
		rs485_sbuf[0] = ACK_ASK_WITH_NOTHING;
}

/****************************************************************************/
extern bool rs485_slave_send(uint8_t dest_addr, uint8_t *data, uint8_t datalen)
{
	if((dest_addr < 0) ||(dest_addr > 128) || (datalen > 64) || (datalen < 0))
		return 0;
	slave_is_txing = 1;
	
	memset(rs485_sbuf, 0x00, BUF_SIZE);

	
	if(MASTER == dest_addr)
		{	
			if(recv_ptr[2] == FUNC_CODE_WRITE)
				{
					rs485_sbuf[0] = ACK_WRITE;
				}	
			else
				rs485_sbuf[0] = ACK_ASK_WITH_DATA_TOMST;
			rs485_sbuf[1] = datalen;
			memcpy(rs485_sbuf+2, data, datalen);
		}
	else 
		{
			rs485_sbuf[0] = ACK_ASK_WITH_DATA_TOSLV;
			rs485_sbuf[1] = dest_addr;
			rs485_sbuf[2] = datalen;
			memcpy(rs485_sbuf+3, data, datalen);
		}
	return 1;
}


/****************************************************************************/
/* need real data */
extern void rs485_slave_recv(uint8_t *recv_ptr )
{
	printf("Slave recieved data from master!\n");
	for(int i = 0; i < recv_ptr[0]; i++)
	{
		printf("%x, ", recv_ptr[i + 1]);
	}
	printf("\n");
}
/****************************************************************************/









/****************************************************************************/


PROCESS_THREAD(rs485_slave_process, ev, data)
{
	PROCESS_BEGIN();

	printf("%s run.\n", rs485_slave_process.name);

	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL( ev == rs485_recv_event );
		recv_ptr = (uint8_t*)data;
		if(recv_ptr[1] == my_slave_addr)
		{
			printf("/*******************************************/\n");
			rs485_slave_recv(recv_ptr );
			
			if(recv_ptr[2] == FUNC_CODE_HANDSHAKE) 
			{
				DO_ACK_HANDSHAKE();
				printf("SLAVE 0x01:I received handshake and acked.\n");
				printf("/*******************************************/\n");
			}
			else if(recv_ptr[2] == FUNC_CODE_ASK)
			{
				/* 1, slave 1 ack master WITH_DATA_TOSLV , slave 2 address is 0x03 */
					
				//uint8_t send_to_slave[MAX_SIZE] = { my_slave_addr, ACK_ASK_WITH_DATA_TOSLV, 0x30, 125,};
				uint8_t send_to_slave[BUF_SIZE+1] = { my_slave_addr,};
				if(rs485_sbuf[0] == ACK_ASK_WITH_DATA_TOSLV)
					{
						memcpy(send_to_slave+1, rs485_sbuf, rs485_sbuf[2]+3);

						rs485_write_data(send_to_slave, send_to_slave[3] + 3);
						slave_is_txing = 0;

						rs485_sbuf_reset();//reset as ACK_ASK_WITH_NOTING
					}
				else if(rs485_sbuf[0] == ACK_ASK_WITH_DATA_TOMST)
					{
						memcpy(send_to_slave+1, rs485_sbuf, rs485_sbuf[1]+2);
						rs485_write_data(send_to_slave, send_to_slave[2] + 3);
						slave_is_txing = 0;
						rs485_sbuf_reset();
					}
				else
					{
						memcpy(send_to_slave+1, rs485_sbuf, 1);
						rs485_write_data(send_to_slave, 2);
					}
					
										
				printf("SLAVE 0x01:recved ASK and acked\n");
				printf("/*******************************************/\n");
			}
			else if(recv_ptr[2] == FUNC_CODE_WRITE)
			{
				if(rs485_sbuf[0] == ACK_WRITE)
					{
						uint8_t ack_write[BUF_SIZE] = {my_slave_addr, };
						memcpy(ack_write+1, rs485_sbuf, rs485_sbuf[1]+2);			
						rs485_write_data(ack_write, ack_write[2] + 2);	
						rs485_sbuf_reset();
						printf("SLAVE 0x01:I received FUNC_CODE_WRITE and acked\n");
						
					}
				else
					{
						uint8_t ack_write[BUF_SIZE] = {my_slave_addr, ACK_WRITE, 1, 1};
						rs485_write_data(ack_write, ack_write[2] + 2);
						printf("SLAVE 0x01:I received FUNC_CODE_WRITE and acked\n");
					}
				printf("/*******************************************/\n");
			}
		}
	}


	PROCESS_END();
}


void rs485_slave_init(uint8_t slave_addr)
{
	my_slave_addr = slave_addr;
	rs485_init();
	process_start(&rs485_slave_process, NULL);
}


