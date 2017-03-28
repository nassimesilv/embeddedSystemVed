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

#include "nrf_gpio.h"
#include <inttypes.h>

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define FILE_NAME_TEMP   "HT16K33_MEAS_TEMP.TXT"
#define TEST_STRING "SD card example.\r\n"

#define HOA2001 14// pin de lecture pour la fourche optique

#define SDC_SCK_PIN     ARDUINO_13_PIN  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN    ARDUINO_11_PIN  ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN    ARDUINO_12_PIN  ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN      ARDUINO_10_PIN  ///< SDC chip select (CS) pin.

/**
 * @brief  SDC block device definition
 * */
NRF_BLOCK_DEV_SDC_DEFINE(
        m_block_dev_sdc,
        NRF_BLOCK_DEV_SDC_CONFIG(
                SDC_SECTOR_SIZE,
                APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
         ),
         NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00")
);

/**
 * @brief Function for demonstrating FAFTS usage.
 */
static void fatfs_example(char* filename, uint8_t value)
{
    static FATFS fs;
    static DIR dir;
    static FILINFO fno;
    static FIL file;

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    NRF_LOG_INFO("Initializing disk 0 (SDC)...\r\n");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed.\r\n");
        return;
    }

    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    NRF_LOG_INFO("Capacity: %d MB\r\n", capacity);

    NRF_LOG_INFO("Mounting volume...\r\n");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        NRF_LOG_INFO("Mount failed.\r\n");
        return;
    }

    
    while (fno.fname[0]);
    NRF_LOG_RAW_INFO("\r\n");

    NRF_LOG_INFO("Writing to file ...\r\n");
    ff_result = f_open(&file, filename, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: .\r\n");
        return;
    }
		char* value2;
		if(value == 0) value2 = "0";
		else value2 = "1";
    ff_result = f_write(&file, value2, sizeof(value2) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.\r\n", bytes_written);
    }

    (void) f_close(&file);
    return;
}

static void gpio_init_HOA2001(void)
{
	nrf_gpio_cfg_input(HOA2001, NRF_GPIO_PIN_NOPULL);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
		uint8_t num_laps;
		uint8_t pin_value;
		
		gpio_init_HOA2001();
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_INFO("\r\nFATFS example.\r\n\r\n");

    

    while (true)
    {
				pin_value = nrf_gpio_pin_read(HOA2001);
			NRF_LOG_INFO("pin value: %u",pin_value);
			if(pin_value == 1) num_laps++;
        fatfs_example("Sensor.txt", pin_value);
    }
}

/** @} */
