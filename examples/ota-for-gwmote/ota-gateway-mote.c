
#include "contiki.h"
#include "ti-lib.h"
#include <stdio.h> /* For printf() */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "spi_api.h"
//#include "net/packetbuf.h"
#include "net/rime/rime.h"
/*---------------------------------------------------------------------------*/
#define MAX_RETRANSMISSIONS 0x03

#define MAX_BUF_SIZE 140
#define MIN_BUF_SIZE 8

#define OTA_START_COMMAND 0x01
#define OTA_SEND_COMMAND 0x02
#define OTA_REQUEST_COMMAND 0x03
/*---------------------------------------------------------------------------*/
extern process_event_t spi_recv_event;
static uint16_t target_addr = 0x0;

static uint8_t radio_rx_buf[MIN_BUF_SIZE] = {0x00, };
static uint8_t spi_rx_len = 0x0;
//static uint8_t spi_tx_len = 0x0;
static uint8_t radio_rx_len = 0x0;
//static uint8_t radio_tx_len = 0x0;
static uint8_t spi_rx_buf[MAX_BUF_SIZE] = {0x00, };
//static uint8_t spi_tx_buf[MIN_BUF_SIZE] = {0x00, };
//static uint8_t radio_tx_buf[MAX_BUF_SIZE] = {0x00, };
/*---------------------------------------------------------------------------*/
PROCESS(spi_read_process, "gateway-mote ota process");
//PROCESS(gwmote_connect_process, "gateway-mote connect process");
//PROCESS(gwmote_send_process, "gateway-mote send image process");

AUTOSTART_PROCESSES(&spi_read_process);
/*---------------------------------------------------------------------------*/
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
	printf("broadcast message received from %x:%x '%s'\n",
			from->u8[0], from->u8[1], (char*)packetbuf_dataptr());
	target_addr = ((from->u8[0])<<8)||(from->u8[1]);
}
/*---------------------------------------------------------------------------*/
static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
	static int i = 0;
	printf("runicast message received from %d.%d, seqno %d\n",
			from->u8[0], from->u8[1], seqno);
	/* if command == OTA_REQUEST_COMMAND, send it to imx6ul by spi_write() */
	memset(radio_rx_buf, 0, sizeof(radio_rx_buf));
	radio_rx_len = packetbuf_copyto(radio_rx_buf);
	if(radio_rx_buf[0] == OTA_REQUEST_COMMAND)
	{
		printf("radio_rx_buf:\n");
		for(i = 0; i < radio_rx_len; i++)
		{
			printf("%d ", radio_rx_buf[i]);
		}
		printf("\n");

		spi_write(radio_rx_buf, radio_rx_len);
	}
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;

static const struct runicast_callbacks runicast_callbacks = {recv_runicast, };
static struct runicast_conn runicast;
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(spi_read_process, ev, data)
{
	//static struct etimer et;
	static int i = 0;

	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
	PROCESS_EXITHANDLER(runicast_close(&runicast);)
	PROCESS_BEGIN();
	broadcast_open(&broadcast, 111, &broadcast_call);
	runicast_open(&runicast, 123, &runicast_callbacks);
	//irq_init();
	//system_init();
	spi_init();
	clock_delay_usec(10);

	if(linkaddr_node_addr.u8[0] == 1 &&
			linkaddr_node_addr.u8[1] == 0)
	{
		PROCESS_WAIT_EVENT_UNTIL(0);
	}

	printf("gateway mote start to run.\n");
	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == spi_recv_event);
		spi_rx_len = spi_read(spi_rx_buf);
		printf("gateway mote received spi data!\n");
		for(i=0; i<spi_rx_len; i++)
		{
			printf("%d ", spi_rx_buf[i]);
		}
		printf("\n");

		if(spi_rx_buf[0] == OTA_START_COMMAND)
		{
			printf("gateway mote is going to send!\n");
			/* 1,broadcast OTA_START_COMMAND out! */
			packetbuf_copyfrom(spi_rx_buf, spi_rx_len);
			broadcast_send(&broadcast);
			printf("OTA_START_COMMAND sent!\n");
		}
		else if(spi_rx_buf[0] == OTA_SEND_COMMAND)
		{
			/* 2,runicast OTA_SEND_COMMAND out with payload! */ 
			if(!runicast_is_transmitting(&runicast))
			{
				linkaddr_t recv;
				memset(spi_rx_buf, 0, sizeof(spi_rx_buf));
				packetbuf_copyfrom(spi_rx_buf, spi_rx_len);
				recv.u8[0] = (target_addr>>8)&&0xff; 
				recv.u8[1] = target_addr&&0xff;

				printf("%u.%u: sending runicast to address %u.%u\n",
						linkaddr_node_addr.u8[0],
						linkaddr_node_addr.u8[1],
						recv.u8[0],
						recv.u8[1]);
				runicast_send(&runicast, &recv, MAX_RETRANSMISSIONS);
			}
		}

		clock_delay_usec(100);

	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
