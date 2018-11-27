/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         This program is for testing
 * \author
 *         www.holliot.com
 */

#include "contiki.h"
#include "ti-lib.h"
#include <stdio.h> /* For printf() */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#include "spi_api.h"
//#include "system-cmd-type.h"
//#include "gateway-mote-broadcast.h"
//#include "gateway-mote-runicast.h"
//#include "ctrl-flow.h"

/*---------------------------------------------------------------------------*/
PROCESS(holliot_gwmote_process, "Holliot gateway-mote process");
AUTOSTART_PROCESSES(&holliot_gwmote_process);
/*---------------------------------------------------------------------------*/



PROCESS_THREAD(holliot_gwmote_process, ev, data)
{

	//PROCESS_EXITHANDLER(runicast_close(&gw_mote_runicast);broadcast_close(&gw_mote_broadcast);)

	PROCESS_BEGIN();
	static uint8_t rx_len = 0;
	static int i = 0;
	static uint8_t rx_buf[32] = {0x00, };
	static uint8_t tx_buf[32] = {0x00, };
	static struct etimer et;

	for(i=0; i<32; i++)
	{
		tx_buf[i] =i;
	}
	system_init();
	spi_init();
	clock_delay_usec(10);
	printf("holliot gateway mote start to run.\n");

	spi_write(tx_buf, 32);
	etimer_set(&et, CLOCK_SECOND * 3); 
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	while(1) 
	{
		//PROCESS_WAIT_EVENT_UNTIL(ev == spi_recv_event);
		etimer_set(&et, CLOCK_SECOND * 3); 
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

#if 0
		printf("gateway mote recved data:\n");
		rx_len = spi_read(rx_buf);

		for(i=0; i<rx_len; i++)
		{
			printf("%x  ", rx_buf[i]);
		}
		printf("\n");
#endif

		printf("gateway mote tx data:\n");
		for(i=0; i<32; i++)
		{
			printf("%x  ", tx_buf[i]);
		}
		printf("\n");
		spi_write(tx_buf, 32);

#if 0

		if(broadcast_flag)
		{
			broadcast_flag = 0x00;
			SendDataByBroadcast(&gw_mote_broadcast, buf_spi_read, spi_len);
			printf("gateway mote: broadcast send.\n");
		}
		else if(runicast_flag)
		{
			runicast_flag = 0x00;
			SendDataByRunicast(&gw_mote_runicast, buf_spi_read, spi_len, mac_buf);
			printf("gateway mote: runicast send.\n");
		}
		else
		{
			printf("WARNING:unkown data.\n");
		}

#endif
	}


	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
