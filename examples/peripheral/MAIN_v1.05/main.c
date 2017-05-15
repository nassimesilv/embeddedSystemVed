//INCLUDE LIBRARIES

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



//ecrit_valeur_fourche_optique_sur_carte_sd

#include "bsp.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

#include "lib/lib_fatfs.c"
#include <stdio.h>
#include "stdio.h"
#include <stdint.h>


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



//VARIABLES

#define FILE_NAME   "MEASURE.csv"
#define SIZE_BUFFER_SD 1000

uint8_t num_laps = 0;
uint32_t buttonBeginPush;									//time when the system isn't started and the button is pushed permit to test if it's a long push
uint32_t buttonBeginPush2;								//time when the system is started and the button is pushed
uint8_t stateSystem = 0; 									// = 0 system isn't started => = 1 system is started  => = 0

uint16_t chronoPrevLapSec = 0;						//store previous lap chrono
uint16_t chronoPrevLapMin = 0;

uint16_t chronoBefore = 0;								//simple variables to test if the chrono have changed after 1 while loop
uint16_t chronoLapBefore = 0;							//idem for the laps chrono
uint8_t speedBefore = 0;                  //idem for speed


float distance = 0; // distance parcourue par la  voiture
float rayon = 0.000254; //rayon roue en km
const float perimeter = 2 * 3.14 *  0.000254;
uint8_t hole = 0; // variable à incrémenter à chaque passage de trou du disque (32 trous)
uint8_t speed;



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
	gpio_init(HOA2001_PIN); 												//fourche optique
	gpio_init(BUTTON_PIN);          								//bouton poussoir

	//Init screens
	NRF_LOG_INFO("screen init : \n");
	initAllScreens();

	//Write 0 on all screens
	NRF_LOG_INFO("clear screens : \n");
	setScreenBegin();

	//Initialize SAADC
	/*saadc_init();
	saadc_sampling_event_init();
	saadc_sampling_event_enable();*/


	//Init timer
	NRF_LOG_INFO("Init Timer\n");
	initTimer();

	NRF_LOG_INFO("End Init\n");
	NRF_LOG_FLUSH();


	while (true)
	{
		//TEST APPUI LONG

		button_value_after = nrf_gpio_pin_read(BUTTON_PIN);

		if (button_value_after==0 && pin_value_before==1 && stateSystem == 0)			//test if button is pushed when the system isn't started
		{
			buttonBeginPush = chronoTotalSec;
		}
		else if (button_value_after==0 && pin_value_before==1 && stateSystem == 1)			//test if button is pushed when system is started
		{

			buttonBeginPush2 = chronoTotalSec;

			printChrono(chronoSec - chronoPrevLapSec, chronoMin - chronoPrevLapMin);     //Print chrono of the laps on the screen
			writeDisplayHT16K33(HT16K33_ADDRESS3);
			nrf_delay_ms(10);

			num_laps++;																																		//Add a lap

			clearHT16K33();																																//Print number of laps on screen
			printLaps(num_laps);
			writeDisplayHT16K33(HT16K33_ADDRESS4);
			nrf_delay_ms(10);

			chronoPrevLapSec = chronoSec;																									//Store the chrono of previous lap
			chronoPrevLapMin = chronoMin;

			chronoLapSec = 0;																															//Restart chrono of the current lap
			chronoLapMin = 0;
		}
		else if (button_value_after==1 && pin_value_before==0 && (chronoTotalSec - buttonBeginPush2) >= 2 && stateSystem == 0)  //if it's a long push and system isn't start we start it
		{
			chronoSec = 0;
			chronoMin = 0;
			chronoLapSec = 0;
			chronoLapMin = 0;

			stateSystem = 1;
		}
		else if (button_value_after==1 && pin_value_before==0 && (chronoTotalSec - buttonBeginPush2) >= 2 && stateSystem == 1)  //if it's a long push and the system is start we stop it
		{
			stateSystem = 0;

			num_laps = 0;

			clearHT16K33();
			setScreenBegin();
		}
		pin_value_before = nrf_gpio_pin_read(BUTTON_PIN);														//Store the value of button to test if there is an invert in next loop


		pin_value = nrf_gpio_pin_read(HOA2001_PIN);																	//Test if a hole is detected by the sensor and increment hole
		if(pin_value == 0 && pin_valueBef ==1) hole++;
		pin_valueBef = nrf_gpio_pin_read(HOA2001_PIN);
		NRF_LOG_INFO("valeur hole : %u \n", hole);

		if(stateSystem == 1)																												//if system is started
		{
			if (chronoTotalMs%1000 == 0)																							//each second we calculate speed
			{
				speed = (distance * 3.6)/timeS;
				NRF_LOG_INFO("speed change to: %d \n", speed);
				distance = 0;
			}
			if (hole % 33 == 0)																												//each 33 holes we calculate distance
			{
				distance += 0.00159512;
				NRF_LOG_INFO("distance change to: %d \n", distance);
			}

			if(speed != speedBefore)																//Print speed only if it's value changed from previous loop and system is begin
			{
				clearHT16K33();
				printLaps(speed);
				writeDisplayHT16K33(HT16K33_ADDRESS5);
				nrf_delay_ms(10);
			}
			speedBefore = speed;


			if(chronoSec!=chronoBefore)															//Print chrono only if it's value changed from previous loop and system is begin
			{
				printChrono(chronoSec, chronoMin);
				writeDisplayHT16K33(HT16K33_ADDRESS);
				nrf_delay_ms(10);
			}
			chronoBefore = chronoSec;

			if(chronoPrevLapSec!= chronoLapBefore)									//Same for lap chrono
			{
				printChrono(chronoLapSec,chronoLapMin);
				writeDisplayHT16K33(HT16K33_ADDRESS2);
				nrf_delay_ms(10);
			}
			chronoLapBefore = chronoLapSec;
		}

		NRF_LOG_FLUSH();
		__WFI();
	}
}
