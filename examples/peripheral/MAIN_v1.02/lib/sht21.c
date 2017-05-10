#include "sht21.h"


/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float GetHumidity(void)
{
	return (-6.0 + 125.0 / 65536.0 * (float)(readSensor(HUMIDITY_HOLD)));
}

/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float GetTemperature(void)
{
	return (-46.85 + 175.72 / 65536.0 * (float)(readSensor(TEMP_HOLD)));
}


uint16_t readSensor(uint8_t command)
{
    uint16_t result;

    uint8_t reg[0];
    reg[0] = command;

    rc = nrf_drv_twi_tx(&m_twi, SHT21_ADDRESS, reg, sizeof(reg), false);
    APP_ERROR_CHECK(rc);
    nrf_delay_ms(100);
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, SHT21_ADDRESS , &m_sample, sizeof(m_sample));
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO(" temp %u", m_sample);
    return m_sample;
}
