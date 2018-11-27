/*
 * Copyright (c) 2018, Holliot (www.holliot.com)
 * All rights reserved.
 */
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_



#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver

/* 1: use rssi value to block out distance, 0: no */
#define	USE_RSSI_BLOCKOUT_DISTANCE		0
#define	RIME_PACKET_RSSI_THRESHOLD		-50

/* currently, the max value is 256*/
#define	SPI_PACKETBUF_CONF_SIZE			128


/* rime packetbuf size: default value is 128*/
#define	PACKETBUF_CONF_SIZE			128



#endif /* PROJECT_CONF_H_ */
