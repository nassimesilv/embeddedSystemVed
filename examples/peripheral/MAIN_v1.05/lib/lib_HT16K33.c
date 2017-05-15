#include "lib_HT16K33.h"


/**
 *  \fn     	setBrightness
 *  \brief  	set the brightness of the screen
 *  \param[in]	uint8_t brightness from 0 to 15
                uint8_t address 8 bit i2c adress
 *  \return 	no return
 */
void setBrightness(uint8_t brightness, uint8_t address)
{
  uint8_t reg[2] = {HT16K33_CMD_BRIGHTNESS | brightness };
  if(brightness>15) brightness=15;
  rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
}

/**
 *  \fn     	blinkRate
 *  \brief  	set the blinkRate of the screen
 *  \param[in]	uint8_t blinkRate from 0 to 3
                uint8_t address 8 bit i2c adress
 *  \return 	no return
 */
void blinkRate(uint8_t blinkRate, uint8_t address)
{
  uint8_t reg[1]={HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (blinkRate<<1) };
  if(blinkRate>3) blinkRate=0;

  rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
}

/**
 *  \fn     	initHT16K33
 *  \brief  	initialize the screen oscillator, blinkRate and brightness
 *  \param[in]	uint8_t address 8 bit i2c adress
 *  \return 	no return
 */
void initHT16K33(uint8_t address)
{
  uint8_t reg[1] = {0x21};
  rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
  nrf_delay_ms(10);
  blinkRate(HT16K33_BLINK_OFF, address);
  nrf_delay_ms(10);
  setBrightness(BRIGHTNESS, address); //max 15
}

/**
 *  \fn     	writeDisplayHT16K33
 *  \brief  	write in the register of the screen, in order to display on the writeDisplayHT16K33 have to be called too
 *  \param[in]	uint8_t address 8 bit i2c adress
 *  \return 	no return
 */
void writeDisplayHT16K33(uint8_t address)
{

  uint8_t reg[11];
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
}

/**
 *  \fn     	clearHT16K33
 *  \brief  	clear the buffer where we store the command before sending it by i2c to the screen
 *  \param[in]	none
 *  \return 	no return
 */
void clearHT16K33(void)
{
  uint8_t i;
  for(i=0; i<8; i++)
  {
    displaybuffer[i]=0;
  }
}

/**
 *  \fn     	writeDigitRawHT16K33
 *  \brief  	write in the buffer where we store the command before sending it by i2c to the screen
 *  \param[in]	uint8_t position position on the screen
                uint8_t bitmask 8-bit raw value we want to write
 *  \return 	no return
 */
void writeDigitRawHT16K33(uint8_t position, uint8_t bitmask) {
  if (position > 5) return;
  displaybuffer[position] = bitmask;
}

/**
 *  \fn     	writeDigitNumHT16K33
 *  \brief  	write a number in the buffer where we store the command before sending it by i2c to the screen
 *  \param[in]	uint8_t position position on the screen
                uint8_t num number we want to write [0-9][a-f]
                bool dot if we want a point at the botom of the digit
 *  \return 	no return
 */
void writeDigitNumHT16K33(uint8_t position, uint8_t num, bool dot) {
  if (position > 5) return;

  writeDigitRawHT16K33(position, numbertable[num] | (dot << 7));
}


/**
 *  \fn     	printErrorHT16K33
 *  \brief  	print error on the screen
 *  \param[in]	none
 *  \return 	no return
 */
void printErrorHT16K33(void) {
  for(uint8_t i = 0; i < SEVENSEG_DIGITS; ++i) {
    writeDigitRawHT16K33(i, (i == 2 ? 0x00 : 0x40));
  }
}

/**
 *  \fn     	drawColonHT16K33
 *  \brief  	store the command to draw a colon the buffer we will send via i2c to the screen
 *  \param[in]	bool state if we want the colon
 *  \return 	no return
 */
void drawColonHT16K33(bool state) {
  if (state)
  displaybuffer[2] = 0x2;
  else
  displaybuffer[2] = 0;
}

/**
 *  \fn     	drawColonHT16K33
 *  \brief  	display a colon on the screen
 *  \param[in]	none
 *  \return 	no return
 */
void writeColonHT16K33(void) {

  uint8_t reg[3];
  reg[0] = 0x04;
  reg[1] = displaybuffer[2] & 0xFF;
  reg[2] = displaybuffer[2] >> 8;

  rc = nrf_drv_twi_tx(&m_twi, HT16K33_ADDRESS, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);

}

/**
 *  \fn     	printChrono
 *  \brief  	print a chrono on the display by storing each digit of a int in a tab, then each case of the tab is turning in an int and written in the write place
              ex: 25 secondes [{2},{5}] write 2 to digit 3 and 5 to digit 4
              Then same for minutes and writes two dots of the chrono (writting any value on the second digit)
 *  \param[in]	uint8_t sec seconds of the chrono
                uint8_t min minuts of the chrono
 *  \return 	no return
 */
void printChrono(uint16_t sec, uint16_t min)
{
  char s[5];
  sprintf(s, "%u\n", sec);

  char m[5];
  sprintf(m, "%u\n", min);

  if(strlen(s) == 2 && strlen(m) == 2)
  {
    char* p;
    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    writeDigitNumHT16K33(5, ret, false);
    writeDigitNumHT16K33(3, 0, false);
    s1[0]=m[0];
    ret = strtol(s1, &p, 10);
    writeDigitNumHT16K33(1, ret, false);
    writeDigitNumHT16K33(0, 0, false);
    writeDigitNumHT16K33(4, 0, false);
  }
  else if(strlen(s) == 3 && strlen(m) == 3)
  {
    char* p;
    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    char s2[1]={s[1]};
    long ret2 = strtol(s2, &p, 10);
    writeDigitNumHT16K33(3, ret, false);
    writeDigitNumHT16K33(5, ret2, false);
    s1[0]=m[0];
    ret = strtol(s1, &p, 10);
    s2[0]=m[1];
    ret2 = strtol(s2, &p, 10);
    writeDigitNumHT16K33(0, ret, false);
    writeDigitNumHT16K33(1, ret2, false);
    writeDigitNumHT16K33(4, 0, false);
  }
  else if(strlen(s) == 2 && strlen(m) == 3)
  {
    char* p;
    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    writeDigitNumHT16K33(3, ret, false);
    writeDigitNumHT16K33(5, 0, false);
    s1[0]=m[0];
    ret = strtol(s1, &p, 10);
    char s2[1]={m[1]};
    long ret2 = strtol(s2, &p, 10);
    writeDigitNumHT16K33(0, ret, false);
    writeDigitNumHT16K33(1, ret2, false);
    writeDigitNumHT16K33(4, 0, false);
  }
  else if(strlen(s) == 3 && strlen(m) == 2)
  {
    char* p;
    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    char s2[1]={s[1]};
    long ret2 = strtol(s2, &p, 10);
    writeDigitNumHT16K33(3, ret, false);
    writeDigitNumHT16K33(5, ret2, false);
    s1[0]=m[0];
    ret = strtol(s1, &p, 10);
    writeDigitNumHT16K33(1, ret, false);
    writeDigitNumHT16K33(0, 0, false);
    writeDigitNumHT16K33(4, 0, false);
  }
}


/**
 *  \fn     	printFloat
 *  \brief  	same as chrono but only for a two digit number (int and not float)
 *  \param[in]	uint16_t n the number
 *  \return 	no return
 */
void printFloat(uint16_t n)
{
		char s[5];
  sprintf(s, "%u\n", n);




  if(strlen(s)-1 == 1)
  {

    char* p;
    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    writeDigitNumHT16K33(5, ret, false);

  }
  else if(strlen(s)-1 == 2)
  {

    char* p;

    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    char s2[1]={s[1]};
    long ret2 = strtol(s2, &p, 10);
    writeDigitNumHT16K33(3, ret, false);
    writeDigitNumHT16K33(5, ret2, false);
  }
}

void printLaps(uint16_t n)
{
		char s[5];
  sprintf(s, "%u\n", n);




  if(strlen(s)-1 == 1)
  {

    char* p;
    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    writeDigitNumHT16K33(1, ret, false);

  }
  else if(strlen(s)-1 == 2)
  {

    char* p;

    char s1[1]={s[0]};
    long ret = strtol(s1, &p, 10);
    char s2[1]={s[1]};
    long ret2 = strtol(s2, &p, 10);
    writeDigitNumHT16K33(0, ret, false);
    writeDigitNumHT16K33(1, ret2, false);
  }
}

/**
 *  \fn     	writeNullChrono
 *  \brief  	write a null chrono in the buffer where we store the command before sending it by i2c to the screen
 *  \param[in]	none
 *  \return 	no return
 */
void writeNullChrono()
{
  writeDigitNumHT16K33(3, 0, false);
  writeDigitNumHT16K33(5, 0, false);
  writeDigitNumHT16K33(1, 0, false);
  writeDigitNumHT16K33(0, 0, false);
  writeDigitNumHT16K33(4, 0, false);
}

/**
 *  \fn     	writeNull
 *  \brief  	write a 0 on the first screen in the buffer where we store the command before sending it by i2c to the screen
 *  \param[in]	none
 *  \return 	no return
 */
void writeNull()
{
  writeDigitNumHT16K33(5, 0, false);
}

/**
 *  \fn     	initAllScreens
 *  \brief  	init all the screens
 *  \param[in]	none
 *  \return 	no return
 */
void initAllScreens()
{
  initHT16K33(HT16K33_ADDRESS);
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS3);
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS4);
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS5);
	nrf_delay_ms(10);
	initHT16K33(HT16K33_ADDRESS6);
	nrf_delay_ms(10);
}

/**
 *  \fn     	setScreenBegin
 *  \brief  	write a null chrono on three screens and a 0 on three other screens
 *  \param[in]	none
 *  \return 	no return
 */
void setScreenBegin()
{
  writeNullChrono();
	writeDisplayHT16K33(HT16K33_ADDRESS);
	nrf_delay_ms(10);
	writeDisplayHT16K33(HT16K33_ADDRESS3);
	nrf_delay_ms(10);
	writeDisplayHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);

	clearHT16K33();
	writeNull();
	writeDisplayHT16K33(HT16K33_ADDRESS4);
	nrf_delay_ms(10);
	writeDisplayHT16K33(HT16K33_ADDRESS5);
	nrf_delay_ms(10);
	writeDisplayHT16K33(HT16K33_ADDRESS6);
	nrf_delay_ms(10);
}
