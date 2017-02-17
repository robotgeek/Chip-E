#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

enum LCD_DISPLAY_TYPE
{
	DRAW_CYLON,
	DRAW_EYES,
	DRAW_HEARTS
};

enum LCD_EYE_POSITIONS
{
	EYES_LEFT,
	EYES_CENTER,
	EYES_RIGHT,
	EYES_UP,
	EYES_DOWN,
	EYES_BLINK
};

extern LiquidCrystal_I2C myLCD;

extern const uint8_t char_heart[8];
extern const uint8_t char_block[8];
extern const uint8_t char_line[8];

void drawCylon();
void drawEyes( int eyePosition );
void drawHearts();

#endif
