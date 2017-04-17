/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
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
 * @defgroup fatfs_example_main main.c
 * @{
 * @ingroup fatfs_example
 * @brief FATFS Example Application main file.
 *
 * This file contains the source code for a sample application using FAT filesystem and SD card library.
 *
 */

#include "nrf.h"
#include "bsp.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"


#include <inttypes.h>

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define FILE_NAME_TEMP   "HT16K33_MEAS_TEMP.TXT"
#define FILE_NAME_FOURCHE "TEST_FOURCHE.txt"
#define TEST_STRING "SD card example.\r\n"
#define SIZE_BUFFER_SD 1000

#define HOA2001 13 // pin de lecture pour la fourche optique

#include "lib/lib_gpio.c"
#include "lib/lib_fatfs.c"
#include <stdio.h>
#include "stdio.h"



/*static void gpio_init_HOA2001(void)
{
	nrf_gpio_cfg_input(HOA2001, NRF_GPIO_PIN_NOPULL);
}*/

/**
 * @brief Function for main application entry.
 */

int main(void)
{
	uint8_t num_laps = 0;
	uint8_t pin_value;
	uint32_t chrono_sd = 0;
	uint8_t buffer_sd[SIZE_BUFFER_SD];
	//char* str;	
	
  gpio_init(HOA2001);
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	NRF_LOG_INFO("\r\nFATFS example.\r\n\r\n");	

	while (true)
	{
		pin_value = nrf_gpio_pin_read(HOA2001);
				
		if (chrono_sd == 10000000) 
		{
			fatfs_example("sensor2.csv", pin_value);
			chrono_sd=0;
		}
		
		NRF_LOG_FLUSH();
		chrono_sd++;
	}
}
