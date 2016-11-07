#include "LCD_Display.h"

LiquidCrystal_I2C myLCD(0x27, 16, 2); //2x16 LCD control @ address 0x27

const uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
const uint8_t char_block[8] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };

bool displayHeartsInverse = false;

int cylonPosition = 0;
bool cylonDirection = true;

void drawCylon()
{
  if ( cylonDirection )
  {
    if ( ++cylonPosition == 16 )
    {
      cylonPosition = 14;
      cylonDirection = false;
    }
  }
  else
  {
    if ( --cylonPosition == -1 )
    {
      cylonPosition = 1;
      cylonDirection = true;
    }
  }

  myLCD.clear();
  if ( cylonDirection )
  {
    myLCD.setCursor(cylonPosition - 1, 0);
    myLCD.write(1); //block
    myLCD.setCursor(cylonPosition - 1, 1);
    myLCD.write(1); //block
  }
  else
  {
    myLCD.setCursor(cylonPosition + 1, 0);
    myLCD.write(1); //block
    myLCD.setCursor(cylonPosition + 1, 1);
    myLCD.write(1); //block
  }
  myLCD.setCursor(cylonPosition, 0);
  myLCD.write(1); //block
  myLCD.setCursor(cylonPosition, 1);
  myLCD.write(1); //block
}

void drawHearts(  )
{
  myLCD.clear();
  for ( int i = 0; i < 15; i += 2 )
  {
    if ( displayHeartsInverse ) myLCD.setCursor(i, 1);
    else myLCD.setCursor(i, 0);
    myLCD.write(0); //heart

    if ( displayHeartsInverse ) myLCD.setCursor(i + 1, 0);
    else myLCD.setCursor(i + 1, 1);
    myLCD.write(0); //heart
  }

  displayHeartsInverse = !displayHeartsInverse;
}
