#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"


//twi fourche optique+affichage3ecrans+bouton poussoir

#include "lib/lib_twi.c"
#include "lib/lib_HT16K33.c"
#include "lib/lib_gpio.c"

#include "nrf_gpio.h"
#include <inttypes.h>

uint8_t pin_value_after;
uint8_t pin_value_before;


//ecrit_valeur_fourche_optique_sur_carte_sd

#include "bsp.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

#include "lib/lib_fatfs.c"
#include <stdio.h>
#include "stdio.h"
#include <stdint.h>

#define FILE_NAME_TEMP   "HT16K33_MEAS_TEMP.TXT"
#define FILE_NAME_FOURCHE "TEST_FOURCHE.txt"
#define TEST_STRING "SD card example.\r\n"
#define SIZE_BUFFER_SD 1000


//capteur courant

#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include <string.h>

#include "lib/lib_cap_courant.c"


//Capteur Humidité température

//#include "lib/sht21.c"


// TIMER

#include <stdbool.h>
#include "lib/lib_timer2.c"


int main(void)
{
	//Init Log system
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	NRF_LOG_INFO("\r\nVinci Eco Drive OnBoard System\r\n\r\n");

	//Init i2c/twi
	NRF_LOG_INFO("twi init : \n");
  twi_init();
	nrf_delay_ms(10);    // Test limit

	//init GPIO
	NRF_LOG_INFO("GPIO init\n");
	gpio_init(HOA2001_PIN); //fourche optique
	gpio_init(14);          //bouton poussoir

	//Init screens
	NRF_LOG_INFO("screen init : \n");
	initAllScreens();

	//Write 0 on all screens
	NRF_LOG_INFO("clear screens : \n");
	writeNullChrono();
	writeDisplayHT16K33(HT16K33_ADDRESS);
	nrf_delay_ms(10);
	writeNullChrono();
	writeDisplayHT16K33(HT16K33_ADDRESS3);
	nrf_delay_ms(10);
	writeNull();
	writeDisplayHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);

	//Initialize SAADC

	//Variables
	NRF_LOG_INFO("Declaring variables : \n");
	uint8_t num_laps;
	uint32_t err_code = NRF_SUCCESS;
	uint32_t chronoNow;
	uint32_t buttonBeginPush;
	uint8_t isChronoBegin = 0;

	//Init timer
	NRF_LOG_INFO("Init Timer\n");
	initTimer();

	NRF_LOG_FLUSH();

	while (true)
  {
		/*
		//Module Begin System
		pin_value_after = nrf_gpio_pin_read(BUTTON);   			//read button value
		if (pin_value_after==0 && pin_value_before==1 && isChronoBegin == 0)			//test if there's an invert on button value
		{
			buttonBeginPush = chrono;
		}
		else if (pin_value_after==1 && pin_value_before==0 && (chrono - buttonBeginPush) > 2 && isChronoBegin == 0 )  //if it's a long push and system isn't start we start it
		{
			chrono = 0;
			num_laps = 0;
			isChronoBegin = 1;
		}
		else if (pin_value_after==1 && pin_value_before==0 && (chrono - buttonBeginPush) >= 2 && isChronoBegin == 1)  //if it's a long push and the system is start we stop it
		{
			isChronoBegin = 2;
		}
		else if(pin_value_after==0 && pin_value_before==1 && isChronoBegin == 1)																		 //if it's a short push we add a lap and fix the chrono on an other screen
		{
			num_laps++;
			chronoLapSec = 0;
			chronoMin = 0;
			printFloat(num_laps);
			writeDisplayHT16K33(HT16K33_ADDRESS2);
			nrf_delay_ms(10);
			printChrono(chronoSec, chronoMin);
			writeDisplayHT16K33(HT16K33_ADDRESS3);
			nrf_delay_ms(10);
		}
		pin_value_before = nrf_gpio_pin_read(BUTTON);				//read button value before to compare

		if(isChronoBegin == 1)															//We only display number of laps and chrono when button is pressed 3 seconds
		{
			printChrono(chronoSec, chronoMin);
			writeDisplayHT16K33(HT16K33_ADDRESS);
			nrf_delay_ms(10);
			//printChrono(chronoLapSec, chronoLapMin);
			//writeDisplayHT16K33(HT16K33_ADDRESS4);
			//nrf_delay_ms(10);*//*

		}
		else if(isChronoBegin == 2)													//Stop scenario
		{
			isChronoBegin = 0;
			writeNullChrono();
			writeDisplayHT16K33(HT16K33_ADDRESS);
			nrf_delay_ms(10);
			writeDisplayHT16K33(HT16K33_ADDRESS3);
			nrf_delay_ms(10);
			writeNull(HT16K33_ADDRESS2);
			writeDisplayHT16K33(HT16K33_ADDRESS2);
			nrf_delay_ms(10);
		}

		//NRF_LOG_INFO("température : %f", GetTemperature());
		//NRF_LOG_INFO("humidité : %f", GetHumidity());
	}*/

	//TEST PUSH BUTTON
	/*
	pin_value_after = nrf_gpio_pin_read(BUTTON);
	writeDigitNumHT16K33(5, pin_value_after, false);
	writeDisplayHT16K33(HT16K33_ADDRESS);
	nrf_delay_ms(10);
	pin_value_before = nrf_gpio_pin_read(BUTTON);
	writeDigitNumHT16K33(5, pin_value_before, false);
	writeDisplayHT16K33(HT16K33_ADDRESS3);
	nrf_delay_ms(10);
	*/

	//TEST PRINT FLOAT
	/*
	num_laps++;
	printFloat(num_laps);
	writeDisplayHT16K33(HT16K33_ADDRESS2);
	*/

	//TEST PRINT CHRONO
	/*
	printChrono(chronoSec, chronoMin);
	writeDisplayHT16K33(HT16K33_ADDRESS3);
	*/

	//TEST ADD LAP
	/*
	pin_value_after = nrf_gpio_pin_read(BUTTON);   			//read button value
	if (pin_value_after==0 && pin_value_before==1)			//test if there's an invert on button value
	{
		num_laps++;
		printFloat(num_laps);
		writeDisplayHT16K33(HT16K33_ADDRESS2);
	}
	pin_value_before = nrf_gpio_pin_read(BUTTON);				//read button value before to compare
	*/

	//TEST STORE chrono
	/*
	printChrono(chronoSec, chronoMin);
	writeDisplayHT16K33(HT16K33_ADDRESS3);

	pin_value_after = nrf_gpio_pin_read(BUTTON);   			//read button value
	if (pin_value_after==0 && pin_value_before==1)			//test if there's an invert on button value
	{
		num_laps++;
		printFloat(num_laps);
		writeDisplayHT16K33(HT16K33_ADDRESS2);
		printChrono(chronoSec, chronoMin);
		writeDisplayHT16K33(HT16K33_ADDRESS);
	}
	*/

	//TEST START STOP system
	/*
	pin_value_after = nrf_gpio_pin_read(BUTTON);   			//read button value
	if (pin_value_after==0 && pin_value_before==1 && isChronoBegin == 0)			//test if there's an invert on button value
	{
		buttonBeginPush = chrono;
	}
	else if (pin_value_after==1 && pin_value_before==0 && (chrono - buttonBeginPush) > 2 && isChronoBegin == 0 )  //if it's a long push and system isn't start we start it
	{
		chrono = 0;
		num_laps = 0;
		isChronoBegin = 1;
	}
	else if (pin_value_after==1 && pin_value_before==0 && (chrono - buttonBeginPush) >= 2 && isChronoBegin == 1)  //if it's a long push and the system is start we stop it
	{
		isChronoBegin = 2;
	}
	else if(pin_value_after==0 && pin_value_before==1 && isChronoBegin == 1)																		 //if it's a short push we add a lap and fix the chrono on an other screen
	{
		num_laps++;
		chronoLapSec = 0;
		chronoMin = 0;
		printFloat(num_laps);
		writeDisplayHT16K33(HT16K33_ADDRESS2);
		nrf_delay_ms(10);
		printChrono(chronoSec, chronoMin);
		writeDisplayHT16K33(HT16K33_ADDRESS3);
		nrf_delay_ms(10);
	}
	pin_value_before = nrf_gpio_pin_read(BUTTON)				//read button value before to compare
	*/

	//TEST FATFS

/*if(isChronoBegin == 1)
{
	char* value;
	char data[]={
		sprintf(value, "%u;", num_laps),
		sprintf(value, "%u;", chrono),
	}
	fatfs_example("test.txt", data);
}*/

	//TEST SAADC
	/*
	__WFE();
	*/



	NRF_LOG_FLUSH();


	}
}
