/***********************************************************************************
 *                  RobotGeek 'Chip-E' Biped - Basic Walking
 *
 *                               ______________
 *                              /              |
 *                             |   _________   |
 *                             |  [_________]  |
 *                             |               |
 *                             |               |
 *                             |               |
 *                             |       O       |
 *                             |               |
 *                              ---------------
 *                                 ||     ||
 *                                 ||     ||
 *                                 ||     ||
 *                            -------     -------
 *                            -------     -------
 *
 *  Wiring
 *
 *    LCD - I2C
 *    Buzzer - Digital Pin 12
 *    IR Receiver - Digital Pin 2
 *    Left Hip Servo - Digital Pin 10
 *    Right Hip Servo - Digital Pin 9
 *    Left Ankle Servo - Digital Pin 6
 *    Right Ankle Servo - Digital Pin 5
 *
 *    Jumper for pins 9/10/11 should be set to 'VIN'
 *    Jumper for pins 3/5/6 should be set to 'VIN'
 *
 *  Control Behavior:
 *
 *    Power on Chip-E for a short demonstration.
 *
 ***********************************************************************************/

#include <ChipE.h>
#include <PiezoEffects.h>
#include "LCD_Display.h"

//--Chip-E
/*
 *          ______________
 *         /              |
 *        |   _________   |
 *        |  [_________]  |
 *        |               |
 *        |               |
 *        |               |
 *        |       O       |
 * YR ==> |               | <== YL
 *         ---------------
 *            ||     ||
 *            ||     ||
 *            ||     ||
 * RR ==>   -----   ------  <== RL
 *          -----   ------
*/
const int PIN_RR = 5; //Roll Right
const int PIN_RL = 6; //Roll Left
const int PIN_YR = 9; //Yaw Right
const int PIN_YL = 10; //Yaw Left
const int TRIM_RR = -1; //Trim on the right ankle (adjust +/- as necessary)
const int TRIM_RL = -1; //Trim on the left ankle (adjust +/- as necessary)
const int TRIM_YR = -1; //Trim on the right hip (adjust +/- as necessary)
const int TRIM_YL = -1; //Trim on the left hip (adjust +/- as necessary)

ChipE myChipE; //Chip-E object (your robot!:)

//--Piezo effects (sounds)
const int BUZZER_PIN = 12;
PiezoEffects mySounds( BUZZER_PIN ); //PiezoEffects object

void setup()
{
  Serial.begin(115200);

  //--LCD initialization
  myLCD.begin();
  myLCD.backlight();
  myLCD.createChar(0, char_heart);
  myLCD.createChar(1, char_block);
  myLCD.createChar(2, char_line);
  drawEyes( EYES_CENTER );

  //--Chip-E initialization
  myChipE.init( PIN_YL, PIN_YR, PIN_RL, PIN_RR );
  myChipE.setTrims( TRIM_YL, TRIM_YR, TRIM_RL, TRIM_RR ); //Set the servo trims
  myChipE.home(); //Move feet to home position
  delay( 500 );

  //--Sound initialization
  mySounds.play( soundHappy );
}

void loop()
{
  processSerialCommand();
  /*
  drawEyes( EYES_CENTER );
  myChipE.home();
  delay( 500 );

  drawEyes( EYES_DOWN );
  myChipE.walk( 4, 2000, FORWARD );
  drawEyes( EYES_CENTER );
  myChipE.walk( 4, 1000, FORWARD );

  myChipE.home();
  mySounds.play( soundWhistle );

  drawEyes( EYES_UP );
  myChipE.walk( 4, 1000, BACKWARD );
  drawEyes( EYES_CENTER );
  myChipE.walk( 4, 2000, BACKWARD );

  myChipE.home();
  mySounds.play( soundWhistle );

  drawEyes( EYES_RIGHT );
  myChipE.turn( 3, 2000, RIGHT );
  drawEyes( EYES_LEFT );
  myChipE.turn( 3, 2000, LEFT );

  drawEyes( EYES_UP );
  mySounds.play( soundUp );
  myChipE.updown( 6, 500, SMALL );
  mySounds.play( soundUp );
  myChipE.updown( 6, 500, MEDIUM );
  drawEyes( EYES_DOWN );
  mySounds.play( soundDown );
  myChipE.updown( 2, 4000, BIG );

  drawEyes( EYES_LEFT );
  myChipE.moonwalker( 3, 2000, 30, LEFT );
  drawEyes( EYES_RIGHT );
  myChipE.moonwalker( 3, 2000, 30, RIGHT );

  drawEyes( EYES_CENTER );
  myChipE.swing( 6, 600, 10 );
  myChipE.swing( 4, 2000, 40 );

  drawEyes( EYES_LEFT );
  myChipE.crusaito( 6, 2000, 40, LEFT );
  drawEyes( EYES_RIGHT );
  myChipE.crusaito( 6, 2000, 40, RIGHT );

  drawEyes( EYES_DOWN );
  myChipE.flapping( 10, 600, 10, FORWARD );
  drawEyes( EYES_UP );
  myChipE.flapping( 10, 600, 10, BACKWARD );

  mySounds.play( soundBeeps );
  */
}

void processSerialCommand()
{
  char buffer[64] = {0};
  int i = 0;

  if ( Serial.available() )
  {
    delay(100); //wait for remaining bytes
    while( Serial.available() && i < sizeof(buffer) )
    {
      buffer[i++] = Serial.read();
    }
    buffer[i++] = '\0';
  }

  if( i > 0 )
  {
    //Compare strings and perform actions
    if (strncmp(buffer, "walk", 4) == 0)
    {
      drawEyes( EYES_DOWN );
      myChipE.walk( 2, 2000, FORWARD );
      drawEyes( EYES_CENTER );
      myChipE.walk( 2, 1000, FORWARD );
      myChipE.home();
      mySounds.play( soundCuddly );
    }
    //Compare strings and perform actions
    if (strncmp(buffer, "hop", 3) == 0)
    {
      drawEyes( EYES_UP );
      mySounds.play( soundUp );
      myChipE.updown( 4, 500, SMALL );
      mySounds.play( soundUp );
      myChipE.updown( 4, 500, MEDIUM );
      drawEyes( EYES_DOWN );
      mySounds.play( soundDown );
      myChipE.updown( 1, 4000, BIG );
      myChipE.home();
    }
    //Compare strings and perform actions
    if (strncmp(buffer, "moonwalk", 8) == 0 || strncmp(buffer, "moon%20walk", 11) == 0)
    {
      drawEyes( EYES_LEFT );
      myChipE.moonwalker( 2, 2000, 30, LEFT );
      drawEyes( EYES_RIGHT );
      myChipE.moonwalker( 2, 2000, 30, RIGHT );
      myChipE.home();
    }
    //Compare strings and perform actions
    if (strncmp(buffer, "turn", 4) == 0)
    {
      drawEyes( EYES_RIGHT );
      myChipE.turn( 3, 2000, RIGHT );
      drawEyes( EYES_LEFT );
      myChipE.turn( 3, 2000, LEFT );
      myChipE.home();
    }
    //Compare strings and perform actions
    if (strncmp(buffer, "sing", 4) == 0)
    {
      mySounds.play( soundBeeps );
    }
    //Compare strings and perform actions
    if (strncmp(buffer, "whistle", 7) == 0)
    {
      mySounds.play( soundWhistle );
    }
  }
}
