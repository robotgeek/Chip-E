/*
Chip-E library derived from https://github.com/JavierIH/zowi
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif

#include "ChipE.h"
#include <Oscillator.h>

void ChipE::init(int YL, int YR, int RL, int RR, bool load_calibration)
{
  servo[0].attach(YL);
  servo[1].attach(YR);
  servo[2].attach(RL);
  servo[3].attach(RR);

  if (load_calibration)
  {
    for (int i = 0; i < 4; i++)
    {
      int servo_trim = EEPROM.read(i);
      if (servo_trim > 128) servo_trim -= 256;
      servo[i].SetTrim(servo_trim);
    }
  }

  for (int i = 0; i < 4; i++) servo_position[i] = 90;
}

void ChipE::setTrims(int YL, int YR, int RL, int RR)
{
  servo[0].SetTrim(YL);
  servo[1].SetTrim(YR);
  servo[2].SetTrim(RL);
  servo[3].SetTrim(RR);
}

void ChipE::saveTrimsOnEEPROM()
{
  for (int i = 0; i < 4; i++) EEPROM.write(i, servo[i].getTrim());
}

void ChipE::moveServos(int time, int  servo_target[])
{
  if(time > 10)
  {
    for (int i = 0; i < 4; i++) increment[i] = ((servo_target[i]) - servo_position[i]) / (time / 10.0);
    final_time =  millis() + time;

    for (int iteration = 1; millis() < final_time; iteration++)
    {
      partial_time = millis() + 10;
      for (int i = 0; i < 4; i++) servo[i].SetPosition(servo_position[i] + (iteration * increment[i]));
      while (millis() < partial_time); //pause
    }
  }
  else
  {
    for (int i = 0; i < 4; i++) servo[i].SetPosition(servo_target[i]);
  }
  for (int i = 0; i < 4; i++) servo_position[i] = servo_target[i];
}

void ChipE::oscillateServos(int A[4], int O[4], int T, double phase_diff[4], float cycle = 1)
{
  for (int i = 0; i < 4; i++)
  {
    servo[i].SetO(O[i]);
    servo[i].SetA(A[i]);
    servo[i].SetT(T);
    servo[i].SetPh(phase_diff[i]);
  }
  double ref = millis();
  for (double x = ref; x <= T * cycle + ref; x = millis())
  {
    for (int i = 0; i < 4; i++)
    {
      servo[i].refresh();
    }
  }
}

//--------------------------------
//-- ChipE at rest position
//--------------------------------
void ChipE::home()
{
  //-- All the parameters are set to 0
  //-- If the amplitudes are 0, there are no oscillation
  int A[4] = {0, 0, 0, 0};
  int O[4] = {0, 0, 0, 0};
  double phase_diff[4] = {0, 0, 0, 0};

  //-- Let's update the oscillators parameters!
  oscillateServos(A, O, 500, phase_diff, 1);
}

void ChipE::_execute(int A[4], int O[4], int T, double phase_diff[4], float steps = 1.0)
{
  int cycles = (int)steps;

  //-- Execute complete cycles
  if (cycles >= 1)
    for(int i = 0; i < cycles; i++)
      oscillateServos(A, O, T, phase_diff);

  //-- Execute the final not complete cycle
  oscillateServos(A, O, T, phase_diff, (float)steps - cycles);
}

//------------------------------------------------
//-- ChipE gait: Walking  (forward or backward)
//-- Parameters:
//--    * steps:  Number of steps
//--    * T : Period
//--    * Dir: Direction: FORWARD / BACKWARD
//------------------------------------------------
void ChipE::walk(float steps, int T, int dir)
{
  //-- Oscillator parameters for walking
  //-- Hip sevos are in phase
  //-- Feet servos are in phase
  //-- Hip and feet are 90 degrees out of phase
  //--      -90 : Walk forward
  //--       90 : Walk backward
  //-- Feet servos also have the same offset (for tiptoe a little bit)
  int A[4] = {30, 30, 20, 20};
  int O[4] = {0, 0, 4, -4};
  double phase_diff[4] = {0, 0, DEG2RAD(dir * -90), DEG2RAD(dir * -90)};

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//----------------------------------------------------
//-- ChipE gait: Turning (left or right)
//-- Parameters:
//--   * Steps: Number of steps
//--   * T: Period
//--   * Dir: Direction: LEFT / RIGHT
//-----------------------------------------------------
void ChipE::turn(float steps, int T, int dir)
{
  //-- Same coordination than for walking (see ChipE::walk)
  //-- The Amplitudes of the hip's oscillators are not igual
  //-- When the right hip servo amplitude is higher, the steps taken by
  //--   the right leg are bigger than the left. So, the robot describes an
  //--   left arc
  int A[4] = {30, 30, 20, 20};
  int O[4] = {0, 0, 4, -4};
  double phase_diff[4] = {0, 0, DEG2RAD(-90), DEG2RAD(-90)};

  if (dir == LEFT)
  {
    A[0] = 30; //-- Left hip servo
    A[1] = 10; //-- Right hip servo
  }
  else
  {
    A[0] = 10;
    A[1] = 30;
  }

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//------------------------------------------------------
//-- ChipE movement: up-down
//--   ChipE moves up and down
//--
//-- Parameters:
//--    * steps: Number of jumps
//--    * T: Period
//--    * h: Jump height: SMALL / MEDIUM / BIG
//--              (or a number in degrees 0 - 90)
//-----------------------------------------------------
void ChipE::updown(float steps, int T, int h)
{
  //-- Both feet are 180 degrees out of phase
  //-- Feet amplitude and offset are the same
  //-- Initial phase for the right foot is -90, so that it starts
  //--   in one extreme position (not in the middle)
  int A[4] = {0, 0, h, h};
  int O[4] = {0, 0, h, -h};
  double phase_diff[4] = {0, 0, DEG2RAD(-90), DEG2RAD(90)};

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-----------------------------------------------------------------
//-- ChipE gait: Moonwalker
//--   ChipE moves like Michael Jackson
//--
//--  Parameters:
//--    Steps: Number of steps
//--    T: Period
//--    h: Height. Typical valures between 15 and 40
//--    dir: Direction: LEFT / RIGHT
//------------------------------------------------------------------
void ChipE::moonwalker(float steps, int T, int h, int dir)
{
  //-- This motion is similar to that of the caterpillar robots: A travelling
  //-- wave moving from one side to another
  //-- The two ChipE's feet are equivalent to a minimal configuration. It is known
  //-- that 2 servos can move like a worm if they are 120 degrees out of phase
  //-- In the example of ChipE, the two feet are mirrored so that we have:
  //--    180 - 120 = 60 degrees. The actual phase difference given to the oscillators
  //--  is 60 degrees.
  //--  Both amplitudes are equal. The offset is half the amplitud plus a little bit of
  //-   offset so that the robot tiptoe lightly

  int A[4] = {0, 0, h, h};
  int O[4] = {0, 0, h / 2 + 2, -h / 2 - 2};
  int phi = -dir * 90;
  double phase_diff[4] = {0, 0, DEG2RAD(phi), DEG2RAD(-60 * dir + phi)};

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//---------------------------------------------------------
//-- ChipE movement: swinging side to side
//--
//--  Parameters:
//--     steps: Number of steps
//--     T : Period
//--     h : Amount of swing (from 0 to 50 aprox)
//-----------------------------------------------------------
void ChipE::swing(float steps, int T, int h)
{

  //-- Both feets are in phase. The offset is half the amplitude
  //-- It causes the robot to swing from side to side
  int A[4] = {0, 0, h, h};
  int O[4] = {0, 0, h / 2, -h / 2};
  double phase_diff[4] = {0, 0, DEG2RAD(0), DEG2RAD(0)};

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-----------------------------------------------------------------
//-- ChipE gait: Crusaito: a mixture between moonwalker and walk
//--
//--   Parameters:
//--     steps: Number of steps
//--     T: Period
//--     h: height (Values between 20 - 50)
//--     dir:  Direction: LEFT / RIGHT
//------------------------------------------------------------------
void ChipE::crusaito(float steps, int T, int h, int dir)
{
  int A[4] = {25, 25, h, h};
  int O[4] = {0, 0, h / 2 + 4, -h / 2 - 4};
  double phase_diff[4] = {90, 90, DEG2RAD(0), DEG2RAD(-60 * dir)};

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-----------------------------------------------------
//-- ChipE gait: Flapping
//--
//--  Parameters:
//--    steps: Number of steps
//--    T: Period
//--    h: height (Values between 10 - 30)
//--    dir: direction: FOREWARD, BACKWARD
//------------------------------------------------------
void ChipE::flapping(float steps, int T, int h, int dir)
{
  int A[4] = {12, 12, h, h};
  int O[4] = {0, 0, h - 10, -h + 10};
  double phase_diff[4] = {DEG2RAD(0), DEG2RAD(180), DEG2RAD(-90 * dir), DEG2RAD(90 * dir)};

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-----------------------------------------------------
//-- ChipE gait: skateboard
//--
//--  Parameters:
//--    steps: Number of pumps
//--    T: Period
//------------------------------------------------------
void ChipE::skateboard(int steps, int T)
{
  //YawL, YawR, RollL, RollR
  int shiftToRight[] = {90, 90, 130, 110};
  moveServos(T, shiftToRight);

  while ( steps-- > 0 )
  {
    int leftFootUp[] = {90, 90, 110, 110};
    moveServos(T / 2, leftFootUp);

    int leftFootForward[] = {90, 70, 110, 110};
    moveServos(T / 2, leftFootForward);

    int leftFootDown[] = {90, 90, 140, 110};
    moveServos(T, leftFootDown);

    int leftFootBackward[] = {90, 110, 145, 110};
    moveServos(T, leftFootBackward);
  }
}
