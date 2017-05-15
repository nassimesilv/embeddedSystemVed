const nrf_drv_timer_t TIMER_LED = NRF_DRV_TIMER_INSTANCE(0);
uint8_t chrono = 0;


/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    static uint32_t i;
	  uint32_t j;
		
    uint32_t led_to_invert = ((i++) % LEDS_NUMBER);

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
					//	chrono++;
				NRF_LOG_INFO("chrono = %u", chrono);
            bsp_board_led_invert(led_to_invert);
						j= j+1;
            break;

        default:
            //Do nothing.
            break;
    }
}


/**
 * @brief Function for main application entry.
 */