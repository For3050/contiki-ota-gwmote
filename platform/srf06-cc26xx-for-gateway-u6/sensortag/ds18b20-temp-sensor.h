
#ifndef _DS18B20_TEMP_SENSOR_H_
#define _DS18B20_TEMP_SENSOR_H_


#include "lib/sensors.h"
#include <sys/clock.h>
#include "contiki.h"


#define DS18B20_COMMAND_READ_SCRATCH_PAD 0xBE
#define DS18B20_COMMAND_START_CONVERSION 0x44
#define DS18B20_COMMAND_SKIP_ROM 0xCC


extern uint8_t ds18b20_probe(void);
extern uint8_t  ds18b20_get_temp(double *temp);
extern uint8_t  ds18b20_get_raw_data(uint8_t *raw_data);
extern uint8_t crc8_ds18b20(uint8_t *buf, uint8_t buf_len);

extern const struct sensors_sensor temp_sensor;

#define TEMP_SENSOR "temp"


#endif /* TEMP_SENSOR_H_ */
