/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "net/netstack.h"
#include "dev/radio.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
#if 0
  static int lqi;
  static short last_lqi_from_attr;
  static int rssi;
  static int last_rssi_from_attr;
  static int test;
  //static int count;
  //static int bigger_count;
  //static int smaller_count;
  printf("==================received===================\n");
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

  NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_LINK_QUALITY, &lqi);
  NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &rssi);
  last_lqi_from_attr = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
  last_rssi_from_attr = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  test = last_rssi_from_attr;
	printf("LQI:%d\n", lqi);
	printf("LQI_FROM_ATTR:%d\n", last_lqi_from_attr);
	printf("RSSI:%d\n", rssi);
	printf("RSSI_FROM_ATTR:%d\n", last_rssi_from_attr);
	printf("test = %d\n", test);

#if 0
  count++;
  if(lqi >= 30)
  {
	printf("LQI:%d\n", lqi);
	bigger_count++;
  }
  else if(lqi < 30)
  {
	printf("LQI:%d\n", lqi);
	smaller_count++;
	printf("WARNNING!!!\n");
  }
	printf("RSSI:%d\n", rssi);
	printf("Count        :%d\n", count);
	printf("Bigger_count :%d\n", bigger_count);
	printf("Smaller_count:%d\n", smaller_count);
#endif
  printf("==================received===================\n");
#endif
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;
  //static radio_result_t rv;
  static int txpower;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 110, &broadcast_call);

  NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &txpower);
  printf("txpower default:%d\n", txpower);
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 5);
  NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &txpower);
  printf("txpower after set 5:%d\n", txpower);

  while(1) {

    /* Delay 3 seconds */
    etimer_set(&et, CLOCK_SECOND * 1);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    printf("==================send===================\n");
    printf("txpower after set 5:%d\n", txpower);
    packetbuf_copyfrom("HelloWorld", 11);
    broadcast_send(&broadcast);
    printf("broadcast message sent\n");
    printf("==================send===================\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/