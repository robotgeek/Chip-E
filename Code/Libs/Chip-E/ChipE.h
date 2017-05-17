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

const int SERVO_COUNT = 6; //4 in legs and 2 arms

class ChipE
{
    public:
        void init(int YL, int YR, int RL, int RR, int LA, int RA, bool load_calibration = 0);
        void setTrims(int YL, int YR, int RL, int RR, int LA, int RA);
        void saveTrimsOnEEPROM();

        void moveServos(int time, int  servo_target[]);
        void oscillateServos(int A[SERVO_COUNT], int O[SERVO_COUNT], int T, double phase_diff[SERVO_COUNT], float cycle);

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
        Oscillator servo[SERVO_COUNT];
        int servo_trim[SERVO_COUNT];
        int servo_position[SERVO_COUNT];

        unsigned long final_time;
        unsigned long partial_time;
        float increment[SERVO_COUNT];

        void _execute(int A[SERVO_COUNT], int O[SERVO_COUNT], int T, double phase_diff[SERVO_COUNT], float steps);
};

#endif
