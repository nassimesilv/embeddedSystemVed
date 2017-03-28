#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "nrf_drv_twi.h"

#define TWI_INSTANCE_ID     0

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

ret_code_t rc;
static volatile bool m_xfer_done = false;
static uint8_t m_sample;
