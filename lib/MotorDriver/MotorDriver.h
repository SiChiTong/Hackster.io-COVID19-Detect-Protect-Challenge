//
// Created by Tatsuya Iwai on 2020/05/05.
// Copyright (c) 2020 greysound.com. All rights reserved.
//

#ifndef SHOECLEANER_MOTORDRIVER_H
#define SHOECLEANER_MOTORDRIVER_H

#include <mbed.h>

//TowerPro SG92R (0.5ms-2.4ms)
#define PWM_PERIOD_MS 20 // period(ms)
#define PWM_DUTY 0.8f // duty cycle (0-1.0f)

namespace greysound {
class MotorDriver {
public:
    PwmOut *enableA;
    DigitalOut *inA1Pin, *inA2Pin;
//    PwmOut *enableB;
//    DigitalOut *inB1Pin, *inB2Pin;

    explicit MotorDriver(PinName _enableA, PinName _inA1=NC, PinName _inA2=NC, PinName _enableB=NC, PinName _inB1=NC, PinName _inB2=NC)
    {
        enableA = new PwmOut(_enableA);
        enableA->write(0);
        enableA->period_ms(PWM_PERIOD_MS);

        inA1Pin = new DigitalOut(_inA1);
        inA1Pin->write(0);

        inA2Pin = new DigitalOut(_inA2);
        inA2Pin->write(0);

//        enableB = new PwmOut(_enableB);
//        enableB->write(PWM_DUTY);
//        enableB->period_ms(PWM_PERIOD_MS);
//
//        inB1Pin = new DigitalOut(_inB1);
//        inB1Pin->write(0);
//        inB2Pin = new DigitalOut(_inB2);
//        inB2Pin->write(0);
    }

    ~MotorDriver()
    {
        delete(inA1Pin);
        delete(inA2Pin);
//        delete(inB1Pin);
//        delete(inB2Pin);
    }

    void forwardA(float _pwm=PWM_DUTY)
    {
        inA1Pin->write(1);
        inA2Pin->write(0);
        enableA->write(_pwm);
    }

    void reverseA(float _pwm=PWM_DUTY)
    {
        inA1Pin->write(0);
        inA2Pin->write(1);
        enableA->write(_pwm);
    }

    void stopA()
    {
        inA1Pin->write(0);
        inA2Pin->write(0);
        enableA->write(0);
    }

private:
};
}

#endif //SHOECLEANER_MOTORDRIVER_H
