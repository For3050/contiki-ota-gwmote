/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "sys/etimer.h"

#include "batmon-sensor.h"

#include "ti-lib.h"
#include "lib/sensors.h"

#include <stdio.h>
#include <stdint.h>
//#include <unistd.h>


#define CC26XX_DEMO_LOOP_INTERVAL       (CLOCK_SECOND * 2)

static struct etimer et;
PROCESS(cc26xx_demo_process, "cc26xx demo process");
AUTOSTART_PROCESSES(&cc26xx_demo_process);


static void
get_tmp_reading()
{
 
   int temp;
   int voltage;
    
   temp = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);

   voltage = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);

   printf("Temp : %d C  &&  voltage: %d mV  \n", temp , (voltage * 125) >> 5 );

}


	static void
init_batmon_reading(void)
{
	SENSORS_ACTIVATE(batmon_sensor);
}


PROCESS_THREAD(cc26xx_demo_process, ev, data)
{

	PROCESS_BEGIN();

	printf("CC26XX demo\n");

	etimer_set(&et, CC26XX_DEMO_LOOP_INTERVAL);
	init_batmon_reading();

	while(1) {

		PROCESS_YIELD();
		if(ev == PROCESS_EVENT_TIMER) 
		{
			get_tmp_reading();

			etimer_set(&et, CC26XX_DEMO_LOOP_INTERVAL);

		}
	}

  PROCESS_END();
}

