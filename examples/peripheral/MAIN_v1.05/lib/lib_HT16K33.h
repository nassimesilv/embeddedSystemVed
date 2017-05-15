#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define HT16K33_ADDRESS           0x70
#define HT16K33_ADDRESS2           0x71
#define HT16K33_ADDRESS3          0x72
#define HT16K33_ADDRESS4          0x73
#define HT16K33_ADDRESS5          0x74
#define HT16K33_ADDRESS6          0x75
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

uint16_t displaybuffer[8];

void blinkRate(uint8_t b, uint8_t address);
void initHT16K33(uint8_t address);
void writeDisplayHT16K33(uint8_t address);
void clearHT16K33(void);
void writeDigitRawHT16K33(uint8_t d, uint8_t bitmask);
void writeDigitNumHT16K33(uint8_t d, uint8_t num, bool dot);
void printErrorHT16K33(void);
void drawColonHT16K33(bool state);
void writeColonHT16K33(void);
