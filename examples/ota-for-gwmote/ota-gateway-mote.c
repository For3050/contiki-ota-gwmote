
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
#define OTA_DOWNLOAD      0x01
#define OTA_ERROR_FRAME   0x02

#define MAX_RETRANSMISSIONS 0x03
#define RADIO_TX_BUF_SIZE 80
#define RADIO_RX_BUF_SIZE 16
#define SPI_TX_BUF_SIZE 16
#define SPI_RX_BUF_SIZE 80

#define PAYLOAD_SIZE 64
/*---------------------------------------------------------------------------*/

static uint8_t radio_tx_buf[RADIO_TX_BUF_SIZE] = {0x00, };
static uint8_t radio_rx_buf[RADIO_RX_BUF_SIZE] = {0x00, };
static uint8_t spi_rx_len = 0x0;
static uint8_t radio_rx_len = 0x0;
static uint8_t spi_tx_buf[SPI_TX_BUF_SIZE] = {0x00, };
static uint8_t spi_rx_buf[SPI_RX_BUF_SIZE] = {0x00, };
/*---------------------------------------------------------------------------*/

extern process_event_t spi_recv_event;

#if 0
struct FRAME
{
	uint8_t func;
	uint16_t frame_id;
	uint16_t total_frame;
	uint8_t length;
	uint8_t payload[PAYLOAD_SIZE];
	uint16_t crc;
};

struct ACK
{
	uint8_t func;
	uint16_t frame_id;
	uint16_t crc;
};

struct FRAME frame;
struct ACK ack;
#endif

/*---------------------------------------------------------------------------*/
PROCESS(spi_read_process, "gateway-mote ota process");
AUTOSTART_PROCESSES(&spi_read_process);

/*---------------------------------------------------------------------------*/
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
	printf("broadcast message received from %x:%x '%s'\n",
			from->u8[0], from->u8[1], (char*)packetbuf_dataptr());
}
/*---------------------------------------------------------------------------*/
static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
	static int i = 0;
	printf("runicast message received from %d.%d, seqno %d\n",
			from->u8[0], from->u8[1], seqno);
	/* if OTA_ERROR, send it to imx6ul by spi_write() */
	memset(radio_rx_buf, 0, sizeof(radio_rx_buf));
	packetbuf_copyto(spi_tx_buf);
	radio_rx_len = 5;
	if(spi_tx_buf[0] == OTA_ERROR_FRAME)
	{
		spi_write(spi_tx_buf, radio_rx_len);

		printf("spi_tx_buf:\n");
		for(i = 0; i < radio_rx_len; i++)
		{
			printf("%d ", spi_tx_buf[i]);
		}
		printf("\n");
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
	static int i = 0;

	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
	PROCESS_EXITHANDLER(runicast_close(&runicast);)
	PROCESS_BEGIN();
	broadcast_open(&broadcast, 129, &broadcast_call);
	runicast_open(&runicast, 144, &runicast_callbacks);
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
		memset(radio_tx_buf, 0, sizeof(radio_tx_buf));
		memcpy(radio_tx_buf, spi_rx_buf, spi_rx_len);
		for(i=0; i<spi_rx_len; i++)
		{
			printf("%d ", radio_tx_buf[i]);
		}
		printf("\n");

		if(radio_tx_buf[0] == OTA_DOWNLOAD)
		{
			printf("func=%d, frame_id=%d, total_frame=%d,length=%d\n",\
					radio_tx_buf[0], (radio_tx_buf[1]<<8)||radio_tx_buf[2],\
					(radio_tx_buf[3]<<8)||radio_tx_buf[4], radio_tx_buf[5]);
			printf("gateway mote is going to send!\n");
			/* 1,broadcast OTA_START_COMMAND out! */
			packetbuf_copyfrom(radio_tx_buf, 8+radio_tx_buf[5]);
			broadcast_send(&broadcast);
			printf("frame sent!\n");
		}
		clock_delay_usec(100);
	}

	PROCESS_END();
}


/*---------------------------------------------------------------------------*/
