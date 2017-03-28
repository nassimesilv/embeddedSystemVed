#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "boards.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_gpio.h"
#include <inttypes.h>

#define HOA2001_PIN 13

uint32_t pin_value;

static void gpio_init(void)
{
        nrf_gpio_cfg_input(HOA2001_PIN, NRF_GPIO_PIN_NOPULL);
}


int main(void)
{
		APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_FLUSH();
    gpio_init();
    while (true)
    {
      pin_value = nrf_gpio_pin_read(HOA2001_PIN);
      NRF_LOG_INFO("pin_value");
		}
}