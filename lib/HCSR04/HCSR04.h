//
// HCSR04.h
// Distance Sensor
// Created by Tatsuya Iwai on 2020/04/27.
// Copyright (c) 2020 greysound.com. All rights reserved.
//

#ifndef SHOECLEANER_HCSR04_H
#define SHOECLEANER_HCSR04_H

#include <mbed.h>
#include "LM60.h"

namespace greysound {

// Number of samples to be stored into buffer
#define BUFFER_LENGTH 5
#define TRIGGER_PERIOD_SEC 0.1f

class HCSR04 {

public:
    double distance;

    /**
     *
     * @param triggerPin
     * @param echoPin
     */
    explicit HCSR04(PinName _trigger, PinName _echo)
    {
        // init vars
        i = 0;
        bufferIndex = 0;
        memset(buffer, 0, BUFFER_LENGTH);
        distance = 0;

        triggerPin = new DigitalOut(_trigger);
        triggerPin->write(0);

        echoPin = new InterruptIn(_echo);
        echoPin->rise(callback(this, &HCSR04::riseEcho)); // trigger echo back start
        echoPin->fall(callback(this, &HCSR04::fallEcho)); // trigger echo back end
    }

    /**
     * destructor
     */
    ~HCSR04()
    {
        delete(echoPin);
        delete(triggerPin);
    }

    void start()
    {
        // stop first
        stop();

        // init vars
        distance = 0;
        bufferIndex = 0;
        memset(buffer, 0, BUFFER_LENGTH);

        // set periodic trigger
        trigger.attach(callback(this, &HCSR04::sendTrigger), TRIGGER_PERIOD_SEC);
    }

    void stop()
    {
        // init trigger
        trigger.detach();
    }

    /**
     * Out pulse to device
     */
    void sendTrigger ()
    {
        triggerPin->write(1);
        wait_us(10); // send high for 10us
        triggerPin->write(0);
    }

private:
    DigitalOut *triggerPin;
    InterruptIn *echoPin;
    Timer timer;
    Ticker trigger;
    uint8_t i;
    double buffer[BUFFER_LENGTH];
    uint8_t bufferIndex;

    /**
     * detect echo back start (LOW -> HIGH)
     */
    void riseEcho()
    {
        // start timer
        timer.start();
    }

    /**
     * detect echo back end (HIGH -> LOW)
     */
    void fallEcho()
    {
        buffer[bufferIndex++] = timer.read_us() * 0.0170;
        timer.stop();
        timer.reset();

        // reset index if exceeds buffer length
        bufferIndex = bufferIndex % BUFFER_LENGTH;

        // calc distance
        calcDistance();
    }

    void calcDistance() {
        distance = 0.0f;
        for (i=0; i<BUFFER_LENGTH; i++) {
            distance += buffer[i];
        }
        distance = distance / BUFFER_LENGTH;
    }
};
}

#endif //SHOECLEANER_HCSR04_H
