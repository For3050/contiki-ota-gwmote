#include "contiki.h"
#include "contiki-lib.h"
#include "spiToUart_driver.h"
#include "hllink.h"

#include <stdio.h>
#include <string.h>
PROCESS(rs485_slave_process, "rs485 link layer slave process");
static uint8_t my_slave_addr = 0x10;

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


PROCESS_THREAD(rs485_slave_process, ev, data)
{
	static uint8_t* recv_ptr;

	PROCESS_BEGIN();

	printf("%s run.\n", rs485_slave_process.name);

	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL( ev == rs485_recv_event );
		recv_ptr = (uint8_t*)data;
		if(recv_ptr[1] == my_slave_addr)
		{
			if(recv_ptr[2] == FUNC_CODE_HANDSHAKE) 
			{
				DO_ACK_HANDSHAKE();
				printf("SLAVE 0x03:I received handshake and acked.\n");
			}
			else if(recv_ptr[2] == FUNC_CODE_ASK)
			{
				/* 1, slave 1 ack master WITH_DATA_TOSLV , slave 2 address is 0x03 */
					
				uint8_t send_to_master[12] = { my_slave_addr, ACK_ASK_WITH_DATA_TOMST, 6, 3, 3, 3, 3, 3, 3, };
				rs485_write_data(send_to_master, send_to_master[2] + 3 );
				
				printf("SLAVE 0x03:recved ASK and acked with DATA_TO_MST.\n");
			}
			else if(recv_ptr[2] == FUNC_CODE_WRITE)
			{
				
				uint8_t ack_write[12] = {my_slave_addr, ACK_WRITE, 0x01, 1,};
					
				printf("SLAVE 0x03:I received FUNC_CODE_WRITE and acked\n");
				for(int i = 0; i < recv_ptr[0]; i++)
					{
						printf("%x\t", recv_ptr[i + 1]);
					}
				printf("\n");
				rs485_write_data(ack_write, 4);
					
			}
		}
	}


	PROCESS_END();
}


void rs485_slave_init(void)
{
	rs485_init();
	process_start(&rs485_slave_process, NULL);
}


