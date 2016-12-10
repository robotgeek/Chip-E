/***********************************************************************************
 *                          RobotGeek 'Chip-E' Biped
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
 *                               -----   ------
 *                               -----   ------
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
 *    Directional buttons (Up, Down, Left, Right) will command the walking gait.
 *    Select and Start buttons perform 'moonwalk'.
 *    TB and TA buttons increase and decrease the gait speed.
 *    B button will make the robot move up and down.
 *    A button will mix 'moonwalk' and walking in a sudo random direction.
 *
 ***********************************************************************************/

#include <IRGamepad.h>
#include <PiezoEffects.h>

#include <ChipE.h>

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
const int TRIM_RR = 0; //Trim on the right ankle (adjust +/- as necessary)
const int TRIM_RL = 0; //Trim on the left ankle (adjust +/- as necessary)
const int TRIM_YR = 0; //Trim on the right hip (adjust +/- as necessary)
const int TRIM_YL = 0; //Trim on the left hip (adjust +/- as necessary)
int currentGaitSpeed = 1000; //The current gait speed in milliseconds.
const int minGaitSpeed = 4000; //The slowest gait speed selectable by gamepad in milliseconds.
const int maxGaitSpeed = 600; //The fastest gait speed selectable by gamepad in milliseconds.
const int gaitSpeedStepSize = 100; //Controls how much the gait speed changes at a time.
bool feetAtHomePosition = true; //Runtime variable to determine if feet should return home.
ChipE myChipE; //Chip-E object (your robot!:)

//--LCD Display
const int lcdDisplayType = DRAW_EYES; //DRAW_CYLON or DRAW_HEARTS or DRAW_EYES
const unsigned long displayUpdateInterval = 3000; //milliseconds defaults: 225 Cylon, 1000 Hearts, 3000 Eyes
unsigned long lastDisplayUpdate = millis();

//--Piezo effects (sounds)
const int BUZZER_PIN = 12;
const bool quietChipE = false; //Set to true if you'd like Chip-E to be a little less chatty
PiezoEffects mySounds( BUZZER_PIN ); //PiezoEffects object

//--Gamepad
const int IR_INPUT_PIN = 2;
const bool useModeB = false; //Set to true if your controller is switched to mode B
unsigned long lastGamepadUpdate = millis();
IR_Gamepad myGamepad(IR_INPUT_PIN, useModeB);

void setup()
{
  Serial.begin(38400);

  //--LCD initialization
  myLCD.begin();
  myLCD.backlight();
  myLCD.createChar(0, char_heart);
  myLCD.createChar(1, char_block);
  myLCD.createChar(2, char_line);

  switch( lcdDisplayType )
  {
  case DRAW_CYLON:
    drawCylon();
    break;
  case DRAW_EYES:
    drawEyes( EYES_CENTER );
    break;
  case DRAW_HEARTS:
    drawHearts();
    break;
  default:
    break;
  }

  //--Chip-E initialization
  myChipE.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR);
  myChipE.setTrims(TRIM_YL, TRIM_YR, TRIM_RL, TRIM_RR); //Set the servo trims
  myChipE.home(); //Move feet to home position
  delay(500);

  //--Sound initialization
  mySounds.play( soundHappy );

  //--Gamepad initialization
  myGamepad.enable();
}

void loop()
{
  //Update diplay when update interval has elapsed
  if ( lastDisplayUpdate + displayUpdateInterval < millis() )
  {
    lastDisplayUpdate = millis();

    switch( lcdDisplayType )
    {
    case DRAW_CYLON:
      drawCylon();
      break;
    case DRAW_EYES:
      if ( feetAtHomePosition ) drawEyes( EYES_BLINK ); //Blink when inactive
      break;
    case DRAW_HEARTS:
      drawHearts();
      break;
    default:
      break;
    }
  }

  //Check gamepad for button presses
  if ( myGamepad.update_button_states() )
  {
    feetAtHomePosition = false;
    lastGamepadUpdate = millis();
    if ( myGamepad.button_press_up() )
    {
      Serial.print( "UP" );
      if ( !quietChipE ) mySounds.play( soundConnection );
      if ( lcdDisplayType == DRAW_EYES ) drawEyes( EYES_DOWN );
      myChipE.walk(1, currentGaitSpeed, FORWARD);
      while ( myGamepad.update_button_states() && myGamepad.button_press_up() )
      {
        myChipE.walk(1, currentGaitSpeed, FORWARD);
      }
    }
    if ( myGamepad.button_press_down() )
    {
      Serial.print( "DOWN" );
      if ( !quietChipE ) mySounds.play( soundButtonPushed );
      if ( lcdDisplayType == DRAW_EYES ) drawEyes( EYES_UP );
      myChipE.walk(1, currentGaitSpeed, BACKWARD);
      while ( myGamepad.update_button_states() && myGamepad.button_press_down() )
      {
        myChipE.walk(1, currentGaitSpeed, BACKWARD);
      }
    }
    if ( myGamepad.button_press_left() )
    {
      Serial.print( "LEFT" );
      if ( lcdDisplayType == DRAW_EYES ) drawEyes( EYES_LEFT );
      myChipE.turn(1, currentGaitSpeed, LEFT);
    }
    if ( myGamepad.button_press_right() )
    {
      Serial.print( "RIGHT" );
      if ( lcdDisplayType == DRAW_EYES ) drawEyes( EYES_RIGHT );
      myChipE.turn(1, currentGaitSpeed, RIGHT);
    }

    if ( myGamepad.button_press_start() )
    {
      Serial.print( "START" );
      myChipE.moonwalker(1, currentGaitSpeed, 30, RIGHT);
    }
    if ( myGamepad.button_press_select() )
    {
      Serial.print( "SELECT" );
      myChipE.moonwalker(1, currentGaitSpeed, 30, LEFT);
    }

    if ( myGamepad.button_press_b() )
    {
      Serial.print( "B" );
      if ( lcdDisplayType == DRAW_EYES ) drawEyes( EYES_UP );
      myChipE.updown(1, 500, SMALL);
    }
    if ( myGamepad.button_press_tb() )
    {
      Serial.print( "TB" );
      if ( currentGaitSpeed > maxGaitSpeed )
      {
        currentGaitSpeed -= gaitSpeedStepSize;
        mySounds.play( soundUp );
      }
      else mySounds.play( soundUhoh );

      delay(100);
      myGamepad.update_button_states();
    }
    if ( myGamepad.button_press_a() )
    {
      Serial.print( "A" );
      if ( millis() % 2 == 0 )
      {
        myChipE.crusaito(1, currentGaitSpeed, 40, LEFT);
        while ( myGamepad.update_button_states() && myGamepad.button_press_a() )
        {
          myChipE.crusaito(1, currentGaitSpeed, 40, LEFT);
        }
      }
      else
      {
        myChipE.crusaito(1, currentGaitSpeed, 40, RIGHT);
        while ( myGamepad.update_button_states() && myGamepad.button_press_a() )
        {
          myChipE.crusaito(1, currentGaitSpeed, 40, RIGHT);
        }
      }
    }
    if ( myGamepad.button_press_ta() )
    {
      Serial.print( "TA" );
      if ( currentGaitSpeed < minGaitSpeed )
      {
        currentGaitSpeed += gaitSpeedStepSize;
        mySounds.play( soundDown );
      }
      else mySounds.play( soundUhoh );

      delay(100);
      myGamepad.update_button_states();
    }

    Serial.println( " button" );
  }
  else if ( lastGamepadUpdate + 3000 < millis() && !feetAtHomePosition )
  {
    if ( lcdDisplayType == DRAW_EYES ) drawEyes( EYES_CENTER );
    myChipE.home();
    if ( !quietChipE ) mySounds.play( soundDisconnection );
    feetAtHomePosition = true;
  }

  delay(20);
}
