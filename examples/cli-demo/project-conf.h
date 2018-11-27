/*
 * Copyright (c) 2018, Holliot (www.holliot.com)
 * All rights reserved.
 */
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver

#define	USE_RSSI_BLOCKOUT_DISTANCE		0
#define	RIME_PACKET_RSSI_THRESHOLD		-50


/* NOTE: currently, our project has 5 sensors, but colloct data sensor count is 3. */
/* currently, max value is 3 */
#define	COLLECTION_DATA_SENSORS_COUNT		3
#define	DEFAULT_DATA_COLLECTION_CYCLE		5


#define	DEST_MAC_ADDR0				0x60
#define	DEST_MAC_ADDR1				0x07


#endif /* PROJECT_CONF_H_ */
