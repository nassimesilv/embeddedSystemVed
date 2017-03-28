static void gpio_init(int pin)
{
        nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL);
}
