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
#include <string.h>
#include "dev/cc26xx-uart.h"
#include "serial-line.h"
#include "leds_driver.h"
#include "sys/clock.h"
#include "sys/rtimer.h"

/*---------------------------------------------------------------------------*/
/* define and declare variables*/
static int16_t array_to_num(char *data);
/*---------------------------------------------------------------------------*/
static int16_t array_to_num(char *data)
{
	static int16_t xabs = 0;
	char first = *data++;
	char second = *data;
	xabs = (first - '0')*10+(second - '0');
	return xabs;
}

/*---------------------------------------------------------------------------*/
PROCESS(holliot_terminal_mote_process, "Holliot terminal mote process");
AUTOSTART_PROCESSES(&holliot_terminal_mote_process);
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(holliot_terminal_mote_process, ev, data)
{

	PROCESS_BEGIN();
	leds_arch_init();
	static int16_t xabs = 0;
	static int16_t ptime = 0;
	static int16_t a = 20;
	static struct etimer et;
	cc26xx_uart_set_input(serial_line_input_byte);
	printf("%s run.\n", holliot_terminal_mote_process.name);
	printf("xabs     range:01--30\n");
	printf("   a     range:5--10--15\n");
	printf("increase a CMD:inc\n");
	printf("decrease a CMD:dec\n");

	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message);
		printf("received from uart:%s\n", (char*)data);
		if(0 == strcmp(data, "inc"))
		{
			a += 1;
			printf("a =%d\n", a);
		}
		else if(0 == strcmp(data, "dec"))
		{
			a -= 1;
			printf("a =%d\n", a);
		}
		else
		{
			printf("enter xabs:01--30\n");
			xabs = array_to_num((char*)data);
			printf("xabs:%d\n",xabs);
			ptime = xabs*a;
			printf("ptime*100(sec)=:%d\n",ptime);
			etimer_set(&et, CLOCK_SECOND*(ptime*0.005));
			turn_on_fan();
			turn_on_led2();
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			turn_off_fan();
			turn_off_led2();
		}
	}


	PROCESS_END();
}





/*---------------------------------------------------------------------------*/
