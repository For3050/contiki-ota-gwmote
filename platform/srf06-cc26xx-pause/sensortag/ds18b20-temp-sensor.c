

#include "contiki.h"
#include "ds18b20-temp-sensor.h"
#include "board.h"
#include "ti-lib.h"
#include <clock.h>

#define delay_us(us)   (_delay_loop_2(1 + (us * F_CPU) / 4000000UL))

void clock_delay_msec(uint16_t howlong);

const struct sensors_sensor temp_mcu_sensor;

/* probe_for_ds18b20 probes for the sensor. Returns 0 on failure, 1 on success
 * Assumptions: only one sensor on the "1-wire bus", on port WSN_DS18B20_PORT
 * BUG: THIS CODE DOES NOT WORK AS INTENDED! IT RETURNS "1" EVEN WHEN THERE
 * IS NO SENSOR CONNECTED.
 */

uint8_t
ds18b20_probe(void)
{
  uint8_t result = 0;

  /* Reset 1W-bus */

  /* Pull PIN low for 480 microseconds (us)
   * Start with setting bit DS18B20_1_PIN to 0 */
  /* then set direction to OUT by setting DS18B20_1_DDR bit to 1 */


  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_DS18B20);
  ti_lib_gpio_clear_dio(BOARD_IOID_DS18B20);

  /* Delay 480-960 us */
  clock_delay_usec(500); /* 480-960 us */


  ti_lib_gpio_set_dio(BOARD_IOID_DS18B20);
  
  /* See if sensor responds. First release the bus and switch to INput mode
   * by setting DS18B20_1_DDR bit to 0 */
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DS18B20);


  /* Activate internal pull-up by  setting pin to HIGH (when in INput mode)
   * OW_SET_PIN_HIGH();
   * Wait for the pin to go HIGH for 64 us */
  clock_delay_usec(64);


  /* Now the sensor, if present, pulls the pin LOW for 60-240 us
   * Detect 0 on PIND bit DS18B20_1_PIN. Invert the result so a presence
   * (aka * a 0)  sets "result" to 1 (for success) */
  result = !ti_lib_gpio_read_dio(BOARD_IOID_DS18B20);

  /* The sensor releases the pin so it goes HIGH after 240 us, add some
     for the signal to stabilize, say 300 usecs to be on the safe side? */
  if(result) {
    clock_delay_usec(400);
    /* Now the bus should be HIGH again */
    result = ti_lib_gpio_read_dio(BOARD_IOID_DS18B20);
  }
  return result;
}
/* Write 1 or 0 on the bus */

void
write_bit(uint8_t bit)
{
  /* Set pin to 0 */
  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_DS18B20);
  ti_lib_gpio_clear_dio(BOARD_IOID_DS18B20);

  /* Pin should be 0 for at least 1 us */
  clock_delay_usec(2);

  /* If we're writing a 1, let interna pull-up pull the bus high
   * within 15 us of setting the bus to low */
  if(bit) {
    /* set 1 */
    ti_lib_gpio_set_dio(BOARD_IOID_DS18B20);
  }
  /* OK, now the bus is either LOW, or pulled HIGH by the internal pull-up
   * Let this state remain for 60 us, then release the bus */
  clock_delay_usec(60);

  /* Release the bus */
  ti_lib_gpio_set_dio(BOARD_IOID_DS18B20);

  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DS18B20);

  /* Allow > 1 us between read/write operations */
  clock_delay_usec(2);
}
/* Read one bit of information from the bus, and return it as 1 or 0 */

uint8_t
read_bit(void)
{
  uint8_t bit = 0;

  /* Set pin to 0 */
  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_DS18B20);
  ti_lib_gpio_clear_dio(BOARD_IOID_DS18B20);

  /* Pin should be 0 for at least 1 us */
  clock_delay_usec(2);
  /* Release the bus */
  ti_lib_gpio_set_dio(BOARD_IOID_DS18B20);

  /* Now read the bus, start by setting in/out direction and activating
   * internal pull-up resistor */
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DS18B20);


  /* ds18b20 either keeps the pin down or releases the bus and the
   * bus then goes high because of the interna pull-up resistor
   * Check whichever happens before 15 us has passed */
  clock_delay_usec(15 - 2 - 1);
  bit = ti_lib_gpio_read_dio(BOARD_IOID_DS18B20);

  /* The complete read cycle must last at least 60 us. We have now spent
   * about 14-15 us in delays, so add another delay to reach >= 60 us */
  clock_delay_usec(50);

  /* Release bus */
  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_DS18B20);
  ti_lib_gpio_set_dio(BOARD_IOID_DS18B20);
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DS18B20);

  /* Allow > 1 us between read/write operations */
  clock_delay_usec(2);

  return bit ? 1 : 0;
}
/* Read one byte of information. A byte is read least significant bit first */

uint8_t
read_byte(void)
{
  uint8_t result = 0;
  uint8_t bit;
  int i;

  for(i = 0; i < 8; i++) {
    bit = read_bit();
    result += (bit << i);
  }
  return result;
}
/* Write one byte of information. A byte is written least significant bit first */

void
write_byte(uint8_t byte)
{
  int i;

  for(i = 0; i < 8; i++) {
    write_bit((byte >> i) & 1);
  }
}
/* ds18b20_get_temp returns the temperature in "temp" (in degrees celsius)
 * Returns 0 on failure (and then "temp" is left unchanged
 * Returns 1 on success, and sets temp */

uint8_t
ds18b20_get_temp(double *temp)
{
  uint8_t result = 0;

  /* Reset bus by probing. Probe returns 1 on success/presence of sensor */
  if(ds18b20_probe()) {
    /* write command "skip rom" since we only have one sensor on the wire! */
    write_byte(DS18B20_COMMAND_SKIP_ROM);

    /* write command to start measurement */
    write_byte(DS18B20_COMMAND_START_CONVERSION);

    /* Wait for conversion to complete. Conversion is 12-bit by default.
     * Since we have external power to the sensor (ie not in "parasitic power"
     * mode) the bus is held LOW by the sensor while the conversion is going
     * on, and then HIGH when conversion is finished. */
    ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DS18B20);
    int count = 0;
    while( !ti_lib_gpio_read_dio(BOARD_IOID_DS18B20) ) {
      //clock_delay_msec(10);
      clock_delay_usec(10000);
      count++;
      /* Longest conversion time is 750 ms (12-bit resolution)
       * So if count > 80 (for a little margin!), we return -274.0
       * which indicates failure to read the temperature. */
      if(count > 80) {
        return 0;
      }
    }

    /* The result is stored in the "scratch pad", a 9 byte memory block.
     * The first two bytes are the conversion result. Reading the scratch pad
     * can be terminated by sending a reset signal (but we read all 9 bytes) */
    (void)ds18b20_probe();
    write_byte(DS18B20_COMMAND_SKIP_ROM);
    write_byte(DS18B20_COMMAND_READ_SCRATCH_PAD);
    uint8_t i, sp_arr[9];
    for(i = 0; i < 9; i++) {
      sp_arr[i] = read_byte();
    }

    /* Check CRC, if mismatch, return 0 (failure to read temperature) */
    uint8_t crc_cal = crc8_ds18b20(sp_arr, 8);

    if(crc_cal != sp_arr[8]) {
      return 0;
    }

    /* OK, now decode what the temperature reading is. This code assumes
     * 12-bit resolution, so this must be modified if the code is modified
     * to use any other resolution! */
    int16_t temp_res;
    uint8_t temp_lsb = sp_arr[0];
    uint8_t temp_msb = sp_arr[1];

    temp_res = (int16_t)temp_msb << 8 | temp_lsb;
    *temp = (double)temp_res * 0.0625;

    result = 1;
  }
  return result;
}


/* crc8 algorithm for ds18b20 */


uint8_t
crc8_ds18b20(uint8_t *buf, uint8_t buf_len)
{
  uint8_t result = 0;
  uint8_t i, b;

  for(i = 0; i < buf_len; i++) {
    result = result ^ buf[i];
    for(b = 1; b < 9; b++) {
      if(result & 0x1) {
        result = (result >> 1) ^ 0x8C;
      } else {
        result = result >> 1;
      }
    }
  }
  return result;
}
static int
value(int type)
{
  double t;
  int ret;
  ret = ds18b20_get_temp(&t);
  
  /* Return temp multiplied by 100 for two decimals */
  if(ret) 
    return (int) (t * 100);

  /* Error return largest negative value */
  return 0x8000;
}
static int
configure(int type, int c)
{
  ds18b20_probe();
  return 0;
}
static int
status(int type)
{
  return 1;
}
SENSORS_SENSOR(temp_sensor, TEMP_SENSOR, value, configure, status);


uint8_t
ds18b20_get_raw_data(uint8_t *raw_data) //add by Pong
{
  uint8_t result = 0;

  /* Reset bus by probing. Probe returns 1 on success/presence of sensor */
  if(ds18b20_probe()) {
    /* write command "skip rom" since we only have one sensor on the wire! */
    write_byte(DS18B20_COMMAND_SKIP_ROM);

    /* write command to start measurement */
    write_byte(DS18B20_COMMAND_START_CONVERSION);

    /* Wait for conversion to complete. Conversion is 12-bit by default.
     * Since we have external power to the sensor (ie not in "parasitic power"
     * mode) the bus is held LOW by the sensor while the conversion is going
     * on, and then HIGH when conversion is finished. */
    ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DS18B20);
    int count = 0;
    while( !ti_lib_gpio_read_dio(BOARD_IOID_DS18B20) ) {
      //clock_delay_msec(10);
      clock_delay_usec(10000);
      count++;
      /* Longest conversion time is 750 ms (12-bit resolution)
       * So if count > 80 (for a little margin!), we return -274.0
       * which indicates failure to read the temperature. */
      if(count > 80) {
        return 0;
      }
    }

    /* The result is stored in the "scratch pad", a 9 byte memory block.
     * The first two bytes are the conversion result. Reading the scratch pad
     * can be terminated by sending a reset signal (but we read all 9 bytes) */
    (void)ds18b20_probe();
    write_byte(DS18B20_COMMAND_SKIP_ROM);
    write_byte(DS18B20_COMMAND_READ_SCRATCH_PAD);
    uint8_t i, sp_arr[9];
    for(i = 0; i < 9; i++) {
      sp_arr[i] = read_byte();
    }

    /* Check CRC, if mismatch, return 0 (failure to read temperature) */
    uint8_t crc_cal = crc8_ds18b20(sp_arr, 8);

    if(crc_cal != sp_arr[8]) {
      return 0;
    }

    /* OK, now decode what the temperature reading is. This code assumes
     * 12-bit resolution, so this must be modified if the code is modified
     * to use any other resolution! */
    //int16_t temp_res;
    uint8_t temp_lsb = sp_arr[0];
    uint8_t temp_msb = sp_arr[1];

	raw_data[0] = sp_arr[1];
	raw_data[1] = sp_arr[0];

    //temp_res = (int16_t)temp_msb << 8 | temp_lsb;

    result = 1;
  }
  return result;
}



