#define HOA2001_PIN 13
#define BUTTON_PIN 14


/**
 *  \fn     	gpio_init
 *  \brief  	Initialize a gpio pin
 *  \param[in]	int pin
 *  \return 	no return
 */
static void gpio_init(int pin)
{
        nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL);
}


/**
 *  \fn     	initAllGpio
 *  \brief  	Initialize the whole GPIO of the onBoard system (optical Fork and button)
 *  \param[in]	none
 *  \return 	no return
 */
static void initAllGpio(void)
{
  gpio_init(HOA2001_PIN); //fourche optique
	gpio_init(BUTTON);          //bouton poussoir
}
