//
// Created by Tatsuya Iwai on 2020/06/27.
// Copyright (c) 2020 greysound.com. All rights reserved.
//

#ifndef SHOECLEANER_PIDCONTROLLER_H
#define SHOECLEANER_PIDCONTROLLER_H

#include <mbed.h>

namespace greysound {

class PIDController {

public:
    float P, I, D, lastP, PID, baseValue;

    PIDController(float kp, float ki, float kd) : Kp(kp), Ki(ki), Kd(kd)
    {
        P = 0;
        I = 0;
        D = 0;
        lastP = 0;
        PID = 0;
        target = 0;
        samplingRate = 0;
        baseValue = 0;
    }

    /**
     * destructor
     */
    ~PIDController()
    {
    }

    void init(float targetValue, float initValue, float samplingRatePerSec=1.0f)
    {
        // init vars
        target = targetValue;
        baseValue = (targetValue - initValue) * Kp;
        samplingRate = samplingRatePerSec;
        P = 0;
        I = 0;
        D = 0;
        lastP = 0;
    }

    void updatePID(float current)
    {
        P = target - current;
        I += P * samplingRate;
        D = (P - lastP) / samplingRate;

        PID = ((Kp * P) + (Ki * I) + (Kd * D)) / baseValue;
        lastP = P;
    }


private:
    float Kp, Ki, Kd;
    float target, samplingRate;

};
}

#endif //SHOECLEANER_PIDCONTROLLER_H
