#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

enum LCD_DISPLAY_TYPE
{
	DRAW_HEARTS,
	DRAW_CYLON
};

extern LiquidCrystal_I2C myLCD;

extern const uint8_t heart[8];
extern const uint8_t char_block[8];

void drawCylon();
void drawHearts();

#endif