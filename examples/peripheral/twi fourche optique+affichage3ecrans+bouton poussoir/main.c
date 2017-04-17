#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "nrf.h"
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_delay.h"

#include "lib/lib_twi.c"
#include "lib/lib_HT16K33.c"
#include "lib/lib_gpio.c"

#include "nrf_gpio.h"
#include <inttypes.h>

#define HOA2001_PIN 13

float pin_value;

uint32_t counter = 0;


int main(void)
{
  uint16_t counter = 0;
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_FLUSH();
  NRF_LOG_INFO("twi init : ");
  twi_init();
  NRF_LOG_INFO("init");
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS);
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);
	gpio_init(14);
	NRF_LOG_FLUSH();
	clearHT16K33();
	printFloat(12);
	writeDisplayHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);
  while (true)
  {
		clearHT16K33();
			pin_value = nrf_gpio_pin_read(14);
			NRF_LOG_INFO("pin_value %u \n", pin_value);
			if(pin_value!=0) pin_value = 1;
			writeDigitNumHT16K33(5, pin_value, false);
			writeDisplayHT16K33(HT16K33_ADDRESS);
			nrf_delay_ms(10);
			NRF_LOG_FLUSH();
			

  }
}
