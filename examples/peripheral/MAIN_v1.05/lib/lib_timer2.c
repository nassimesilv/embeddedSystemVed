const nrf_drv_timer_t TIMER = NRF_DRV_TIMER_INSTANCE(0);
uint8_t chronoSec = 0;
uint8_t chronoMin = 0;
uint32_t chronoTotalMs = 0;
uint32_t chronoTotalSec = 0;
uint8_t chronoLapSec = 0;
uint8_t chronoLapMin = 0;
uint32_t time_ms = 1; //Time(in miliseconds) between consecutive compare events.
uint32_t time_ticks;
/**
 * @brief Handler for timer events.
 */

void timer_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    static uint32_t i;
	  uint32_t j;

    uint32_t led_to_invert = ((i++) % LEDS_NUMBER); //Test phase to erase

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
						if(chronoTotalMs%1000==0)
					{
            if(chronoSec < 59)
            {
              chronoSec++;
              chronoLapSec++;
            }
            else
            {
              chronoSec = 0;
              chronoLapSec = 0;
              chronoMin++;
              chronoLapMin++;
            }
						chronoTotalSec++;
					}
					chronoTotalMs++;

						j= j+1;

            break;

        default:
            //Do nothing.
            break;
    }
}


void initTimer()
{
	ret_code_t err_code;
  //Configure all leds on board. Test phase to erase in the final system
	bsp_board_leds_init();

	//Configure TIMER for chrono
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	err_code = nrf_drv_timer_init(&TIMER, &timer_cfg, timer_event_handler);
	APP_ERROR_CHECK(err_code);

	time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER, time_ms);

	nrf_drv_timer_extended_compare(
			 &TIMER, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

	nrf_drv_timer_enable(&TIMER);
}
