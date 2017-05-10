#define HOA2001_PIN 13
#define BUTTON 14

static void gpio_init(int pin)
{
        nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL);
}

static void initAllGpio(void)
{
  gpio_init(HOA2001_PIN); //fourche optique
	gpio_init(BUTTON);          //bouton poussoir
}
