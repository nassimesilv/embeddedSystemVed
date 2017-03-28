#include <stdbool.h>
#include "nrf.h"

#include "nrf_gpio.h"
#include <inttypes.h>


#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include <stdlib.h>
#include <string.h>

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#define HOA2001_PIN 13
/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

#define HT16K33_ADDRESS           0x71
#define HT16K33_ADDRESS2           0x72
#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01


#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3



#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

static const uint8_t numbertable[] = {
	0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6F, /* 9 */
	0x77, /* a */
	0x7C, /* b */
	0x39, /* C */
	0x5E, /* d */
	0x79, /* E */
	0x71, /* F */
};
uint8_t pin_value;
uint16_t displaybuffer[8];
ret_code_t rc;
static volatile bool m_xfer_done = false;
uint32_t counter = 0;


/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;


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

void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_lm75b_config = {
       .scl                = ARDUINO_SCL_PIN,
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_lm75b_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

void setBrightness(uint8_t b, uint8_t address)
{
	uint8_t reg[2] = {HT16K33_CMD_BRIGHTNESS | b };
	if(b>15) b=15;
	rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
	APP_ERROR_CHECK(rc);
}

void blinkRate(uint8_t b, uint8_t address)
{
	NRF_LOG_INFO("reg");
	uint8_t reg[1]={HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b<<1) };
	NRF_LOG_INFO("if");
	if(b>3) b=0;
	NRF_LOG_INFO("rc");

	rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
	NRF_LOG_INFO("check");
	APP_ERROR_CHECK(rc);
	NRF_LOG_INFO("endblinkrate");
}

void init(uint8_t address)
{
	uint8_t reg[1] = {0x21};
	rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
	APP_ERROR_CHECK(rc);
	nrf_delay_us(220);
	blinkRate(0, address);
	nrf_delay_us(220);
	setBrightness(15, address); //max 15
}

void writeDisplay(uint8_t address)
{
		uint8_t reg2[1];
		uint8_t reg[11];
		uint8_t i;
		reg[0] = (uint8_t)0x00;

		reg[1] = displaybuffer[0];
		reg[2] = displaybuffer[0] >> 8;

		reg[3] = displaybuffer[1];
		reg[4] = displaybuffer[1] >> 8;

		reg[5] = displaybuffer[4];
		reg[6] = displaybuffer[4] >> 8;

		reg[7] = displaybuffer[3];
		reg[8] = displaybuffer[3] >> 8;

		reg[9] = displaybuffer[5];
		reg[10] = displaybuffer[5] >> 8;

		rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), true);
		APP_ERROR_CHECK(rc);
		nrf_delay_us(1200);
}

void clear(void)
{
	uint8_t i;
	for(i=0; i<8; i++)
	{
		displaybuffer[i]=0;
	}
}

void writeDigitRaw(uint8_t d, uint8_t bitmask) {
  if (d > 5) return;
  displaybuffer[d] = bitmask;
}

void writeDigitNum(uint8_t d, uint8_t num, bool dot) {
  if (d > 5) return;

  writeDigitRaw(d, numbertable[num] | (dot << 7));
}



void printError(void) {
  for(uint8_t i = 0; i < SEVENSEG_DIGITS; ++i) {
    writeDigitRaw(i, (i == 2 ? 0x00 : 0x40));
  }
}

void drawColon(bool state) {
  if (state)
    displaybuffer[2] = 0x2;
  else
    displaybuffer[2] = 0;
}

void writeColon(void) {

		uint8_t reg[3];
		reg[0] = 0x04;
	    reg[1] = displaybuffer[2] & 0xFF;
	    reg[2] = displaybuffer[2] >> 8;


    rc = nrf_drv_twi_tx(&m_twi, HT16K33_ADDRESS, reg, sizeof(reg), false);
    APP_ERROR_CHECK(rc);

}


static void gpio_init(void)
{
        nrf_gpio_cfg_input(HOA2001_PIN, NRF_GPIO_PIN_NOPULL);
}

void printFloat(double n) 
{ 
	uint8_t base = 10;
	uint8_t fracDigits = 0;
  uint8_t numericDigits = 4;   // available digits on display
  bool isNegative = false;  // true if the number is negative
  
  // is the number negative?
  if(n < 0) {
    isNegative = true;  // need to draw sign later
    numericDigits = numericDigits -1;    // the sign will take up one digit
    n = n * (-1);            // pretend the number is positive
  }
  
  // calculate the factor required to shift all fractional digits
  // into the integer part of the number
  double toIntFactor = 1.0;
	int i;
  for(i = 0; i < fracDigits; ++i) toIntFactor = toIntFactor * base;
  
  // create integer containing digits to display by applying
  // shifting factor and rounding adjustment
  uint32_t displayNumber = n * toIntFactor + 0.5;
  
  // calculate upper bound on displayNumber given
  // available digits on display
  uint32_t tooBig = 1;
  for(i = 0; i < numericDigits; ++i) tooBig = tooBig * base;
  
  // if displayNumber is too large, try fewer fractional digits
  while(displayNumber >= tooBig) {
    fracDigits = fracDigits - 1;
    toIntFactor = toIntFactor / base;
    displayNumber = n * toIntFactor + 0.5;
  }
  
  // did toIntFactor shift the decimal off the display?
  if (toIntFactor < 1) {
    printError();
  } else {
    // otherwise, display the number
    int8_t displayPos = 4;
    
    if (displayNumber)  //if displayNumber is not 0
    {
			uint8_t j;
			displayPos = displayPos - 1;
			/*if(displayPos == 3) displayPos = 4;
			if(displayPos == 2) displayPos = 3;*/
      for(j = 0; displayNumber || j <= fracDigits; ++j) {
        bool displayDecimal = (fracDigits != 0 && j == fracDigits);
        writeDigitNum(displayPos, displayNumber % base, displayDecimal);
        if(displayPos == 2) writeDigitRaw(displayPos, 0x00);
        displayNumber = displayNumber / base;
      }
    }
    else {
			displayPos = displayPos - 1;
      writeDigitNum(displayPos, 0, false);
    }
  
    // display negative sign if negative
    if(isNegative){
			displayPos = displayPos - 1;
			writeDigitRaw(displayPos, 0x40);
		}
  
    // clear remaining display positions
    while(displayPos >= 0){
			displayPos = displayPos - 1;
			writeDigitRaw(displayPos, 0x00);
		}
  }
}


int main(void)
{
  uint16_t counter = 0;
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_FLUSH();
  NRF_LOG_INFO("twi init : ");
  twi_init();
		nrf_delay_us(323);

  NRF_LOG_INFO("init");
  init(HT16K33_ADDRESS);
	nrf_delay_us(323);
	//init(HT16K33_ADDRESS2);
	gpio_init();
		nrf_delay_us(323);

	NRF_LOG_FLUSH();


  while (true)
  {
		clear();
			/*pin_value = nrf_gpio_pin_read(HOA2001_PIN);
			NRF_LOG_INFO("pin_value");
			writeDigitNum(0, pin_value, false);
		writeDisplay(HT16K33_ADDRESS);
		clear();
		writeDigitNum(0, (counter / 1000), false);
writeDigitNum(1, (counter / 100) % 10, false);
writeDigitNum(4, (counter / 10) % 10, false);
writeDigitNum(3, (counter / 10) % 10, false);
writeDigitNum(5, counter % 10, false);
			writeDisplay(HT16K33_ADDRESS2);
			counter++;*/
		writeDigitNum(0, 5, false);
		writeDisplay(HT16K33_ADDRESS);
  }
}
