/*
Chip-E library derived from https://github.com/JavierIH/zowi
*/

#ifndef ChipE_h
#define ChipE_h

#include <Servo.h>
#include <Oscillator.h>
#include <EEPROM.h>

//-- Constants
const int FORWARD = 1;
const int BACKWARD = -1;
const int LEFT = 1;
const int RIGHT = -1;
const int SMALL = 5;
const int MEDIUM = 15;
const int BIG = 30;

class ChipE
{
    public:
        void init(int YL, int YR, int RL, int RR, bool load_calibration = 0);
        void setTrims(int YL, int YR, int RL, int RR);
        void saveTrimsOnEEPROM();

        void moveServos(int time, int  servo_target[]);
        void oscillateServos(int A[4], int O[4], int T, double phase_diff[4], float cycle);

        void home();
        void walk(float steps = 4, int T = 1800, int dir = FORWARD);
        void turn(float steps = 1, int T = 2000, int dir = LEFT);
        void updown(float steps = 1, int T = 1000, int h = 20);
        void moonwalker(float steps, int T, int h, int dir);
        void swing(float steps, int T, int h);
        void crusaito(float steps, int T, int h, int dir);
        void flapping(float steps, int T, int h, int dir);
        void skateboard(int steps = 1, int T = 8000);

    private:
        Oscillator servo[4];
        int servo_trim[4];
        int servo_position[4];

        unsigned long final_time;
        unsigned long partial_time;
        float increment[4];

        void _execute(int A[4], int O[4], int T, double phase_diff[4], float steps);
};

#endif
