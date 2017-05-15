#include "lib_HT16K33.h"

void setBrightness(uint8_t brightness, uint8_t address)
{
  uint8_t reg[2] = {HT16K33_CMD_BRIGHTNESS | brightness };
  if(brightness>15) brightness=15;
  rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
}

void blinkRate(uint8_t blinkRate, uint8_t address)
{
  uint8_t reg[1]={HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (blinkRate<<1) };
  if(blinkRate>3) blinkRate=0;

  rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
}

void initHT16K33(uint8_t address)
{
  uint8_t reg[1] = {0x21};
  rc = nrf_drv_twi_tx(&m_twi, address, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);
  nrf_delay_ms(10);
  blinkRate(0, address);
  nrf_delay_ms(10);
  setBrightness(15, address); //max 15
}

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

void clearHT16K33(void)
{
  uint8_t i;
  for(i=0; i<8; i++)
  {
    displaybuffer[i]=0;
  }
}

void writeDigitRawHT16K33(uint8_t position, uint8_t bitmask) {
  if (position > 5) return;
  displaybuffer[position] = bitmask;
}

void writeDigitNumHT16K33(uint8_t position, uint8_t num, bool dot) {
  if (position > 5) return;

  writeDigitRawHT16K33(position, numbertable[num] | (dot << 7));
}



void printErrorHT16K33(void) {
  for(uint8_t i = 0; i < SEVENSEG_DIGITS; ++i) {
    writeDigitRawHT16K33(i, (i == 2 ? 0x00 : 0x40));
  }
}

void drawColonHT16K33(bool state) {
  if (state)
  displaybuffer[2] = 0x2;
  else
  displaybuffer[2] = 0;
}

void writeColonHT16K33(void) {

  uint8_t reg[3];
  reg[0] = 0x04;
  reg[1] = displaybuffer[2] & 0xFF;
  reg[2] = displaybuffer[2] >> 8;

  rc = nrf_drv_twi_tx(&m_twi, HT16K33_ADDRESS, reg, sizeof(reg), false);
  APP_ERROR_CHECK(rc);

}

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

void writeNullChrono()
{
  writeDigitNumHT16K33(3, 0, false);
  writeDigitNumHT16K33(5, 0, false);
  writeDigitNumHT16K33(1, 0, false);
  writeDigitNumHT16K33(0, 0, false);
  writeDigitNumHT16K33(4, 0, false);
}

void writeNull()
{
  writeDigitNumHT16K33(5, 0, false);
}


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

void clearAllScreens()
{
  clearHT16K33();
	writeDisplayHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);
	writeDisplayHT16K33(HT16K33_ADDRESS2);
	nrf_delay_ms(10);
}

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
