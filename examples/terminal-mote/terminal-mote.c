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
 *         This program is for holliot sensor mote
 * \author
 *         www.holliot.com
 */

#include "contiki.h"
#include "ti-lib.h"
#include <stdio.h> /* For printf() */


#include "system-cmd-type.h"
#include "ctrl-flow.h"
#include "sensor-mote-broadcast.h"
#include "sensor-mote-runicast.h"
#include "pir-sensor.h"
#include "light-sensor.h"
#include "leds_driver.h"
/*---------------------------------------------------------------------------*/
/* define and declare variables*/
/*---------------------------------------------------------------------------*/


PROCESS_NAME(data_collection_process);
/*---------------------------------------------------------------------------*/
PROCESS(holliot_terminal_mote_process, "Holliot terminal mote process");
AUTOSTART_PROCESSES(&holliot_terminal_mote_process);
/*---------------------------------------------------------------------------*/
PROCESS(monitor_pir_process, "monitor pir process");
PROCESS(monitor_light_process, "monitor light process");

#define	DATA_SIZE	32
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(holliot_terminal_mote_process, ev, data)
{

	PROCESS_EXITHANDLER(runicast_close(&sensor_mote_runicast);)

	static uint8_t dataload[DATA_SIZE];
	uint8_t len = 0x00;
	
	PROCESS_BEGIN();
	process_start(&monitor_pir_process, NULL);
	process_start(&monitor_light_process, NULL);

	system_init();
	/* Receiver node: do nothing *//***************************************/
	if(linkaddr_node_addr.u8[0] == 1 && linkaddr_node_addr.u8[1] == 0) 
	{
		PROCESS_WAIT_EVENT_UNTIL(0);
	}

	printf("%s run.\n", holliot_terminal_mote_process.name);

	while(1) 
	{
		/* Delay  */
		
		PROCESS_WAIT_EVENT_UNTIL(ev == recv_command_event);
		

		if( 0x01 == system_config_cmd )	//SYSTEM_CONFIG 
		{
			printf("recv SYSTEM_CONFIGURE!\n");
            		parse_config_cmd(runicast_recv_buf);
			system_config_cmd = 0x00;
		}
		if( 0x01 == system_run_cmd )   //SYSTEM_RUN
		{
			printf("recv SYSTEM RUN!\n");
			if( !(process_is_running(&data_collection_process)) )
			{
				printf("start data collection process!\n");
				process_start(&data_collection_process, NULL);
				
			}
			else
			{
				printf("data collection process is already running.\n");
			}

			system_run_cmd = 0x00;
		}
		if( 0x01 == system_query_cmd )   //SYSTEM_QUERY
		{
			memset(dataload, 0, DATA_SIZE);
			if(system_query_device_cmd == 0x01)
			{
				len = FormatQueryDevData(dataload);
				SendDataByRunicast(&sensor_mote_runicast, dataload, len);
				system_query_device_cmd = 0x00;
			}
			else if(system_query_sensor_cmd == 0x01)
			{
				len = FormatQuerySensorData(dataload);
				SendDataByRunicast(&sensor_mote_runicast, dataload, len);
				system_query_sensor_cmd = 0x00;
			}
			system_query_cmd = 0x00;
		}



	}


	PROCESS_END();
}


PROCESS_THREAD(monitor_pir_process, ev, data)
{
	static struct timer led1_timer;
	static uint8_t count=5;
	static uint8_t index=1;
	PROCESS_BEGIN();
	printf("%s start\n",monitor_pir_process.name);
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == pir_change_event);
		printf("\n%d:WARNING: someone broke in !!!\n",index);
		index++;
		while(count--)
		{
			turn_off_led2();
			turn_on_led1();
			timer_set(&led1_timer, CLOCK_SECOND >> 2);
			while( !(timer_expired(&led1_timer)) );
			turn_off_led1();
			turn_on_led2();
			timer_set(&led1_timer, CLOCK_SECOND >> 2);
			while( !(timer_expired(&led1_timer)) );
		}
		count = 5;
	}
	PROCESS_END();
}


PROCESS_THREAD(monitor_light_process, ev, data)
{
	PROCESS_BEGIN();
	printf("%s start\n",monitor_light_process.name);
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == light_change_event);
		printf("\nWARNING: light status changed !!!\n");
	}
	PROCESS_END();
}



/*---------------------------------------------------------------------------*/
