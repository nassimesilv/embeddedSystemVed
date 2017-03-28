/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @defgroup tw_sensor_example main.c
 * @{
 * @ingroup nrf_twi_example
 * @brief TWI Sensor Example main file.
 *
 * This file contains the source code for a sample application using TWI.
 *
 */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Common addresses definition for temperature sensor. */
#define HT16K33_ADDR (0x40)

#define HT16K33_INIT_OSCILL 0x21
#define HT16K33_MEAS_TEMP   0xE3

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;

/**
 * @brief Function for setting active mode on HT16K33.
 */
void HT16K33_set_mode(void)
{
  ret_code_t rc;

  uint8_t reg[1] = {HT16K33_INIT_OSCILL};
  rc = nrf_drv_twi_tx(&m_twi, HT16K33_ADDR, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
  while(m_xfer_done ==false);
}

/**
 * @brief Function for handling data from temperature sensor.
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
__STATIC_INLINE void data_handler(uint8_t temp)
{
    NRF_LOG_INFO("Temperature: %d Celsius degrees.\r\n", temp);
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                data_handler(m_sample);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}

/**
 * @brief UART initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_ht16k33_config = {
       .scl                = ARDUINO_SCL_PIN,
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_ht16k33_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}


static void start_temp_meas()
{
  m_xfer_done = false;

	  uint8_t reg[1] = {HT16K33_MEAS_TEMP};

  ret_code_t rc = nrf_drv_twi_tx(&m_twi, HT16K33_ADDR, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
}
/**
 * @brief Function for reading data from temperature sensor.
 */
static void read_sensor_data()
{
    m_xfer_done = false;

    /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, HT16K33_ADDR, &m_sample, sizeof(m_sample));
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    NRF_LOG_INFO("\r\nTWI sensor example\r\n");
    NRF_LOG_FLUSH();
    twi_init();
    nrf_delay_ms(100);
    //HT16K33_set_mode();
    //nrf_delay_ms(10);

    while (true)
    {
        nrf_delay_ms(500);

       /* do
        {
            __WFE();
        }while (m_xfer_done == false);
        */
			start_temp_meas();
        nrf_delay_ms(10);
        read_sensor_data();
        NRF_LOG_FLUSH();
    }
}

/** @} */
