#include "contiki.h"
#include "dev/leds.h"

#include "ti-lib.h"
/*---------------------------------------------------------------------------*/
static int inited = 0;
/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  if(inited) {
    return;
  }
  inited = 1;

  ti_lib_rom_ioc_pin_type_gpio_output(BOARD_IOID_LED_1);
  ti_lib_rom_ioc_pin_type_gpio_output(BOARD_IOID_LED_2_TDO);
  ti_lib_rom_ioc_pin_type_gpio_output(BOARD_IOID_FAN);

  ti_lib_gpio_clear_multi_dio(BOARD_LED_ALL);
	ti_lib_gpio_clear_dio(BOARD_IOID_FAN);
}
/*---------------------------------------------------------------------------*/
void turn_on_fan(void)
{
	ti_lib_gpio_set_dio(BOARD_IOID_FAN);
}

void turn_off_fan(void)
{
	ti_lib_gpio_clear_dio(BOARD_IOID_FAN);
}

void turn_on_led1(void)
{
	ti_lib_gpio_set_dio(BOARD_IOID_LED_1);
}

void turn_off_led1(void)
{
	ti_lib_gpio_clear_dio(BOARD_IOID_LED_1);
}

void turn_on_led2(void)
{
	ti_lib_gpio_set_dio(BOARD_IOID_LED_2_TDO);
}

void turn_off_led2(void)
{
	ti_lib_gpio_clear_dio(BOARD_IOID_LED_2_TDO);
}
