//
// Created by Tatsuya Iwai on 2020/05/10.
// Copyright (c) 2020 greysound.com. All rights reserved.
//

#ifndef SHOECLEANER_LM60_H
#define SHOECLEANER_LM60_H

#include <mbed.h>

namespace greysound {

#define LM60_PERIOD_SEC 0.5f // updates every 0.5 sec

class LM60 {

public:
    double temperature;

    explicit LM60(PinName _pin)
    {
        temperature = 0.0f;
        lm60 = new AnalogIn(_pin);
    }

    /**
     * destructor
     */
    ~LM60()
    {
        delete(lm60);
    }

    void start()
    {
        // stop first
        stop();

        // init vars
        temperature = 0.0f;

        // set periodic trigger
        trigger.attach(callback(this, &LM60::readTemp), LM60_PERIOD_SEC);
    }

    void stop()
    {
        // init trigger
        trigger.detach();
    }

    void readTemp()
    {
        /**
         * LM60 outputs voltage based on its temperature (-40 to 125C)
         * Offset: 0.424 mV @ 0C
         * Resolution: 6.25 mV/C
         *
         * examples:
         * 125C = 1.205 mV
         * 100C = 1.049 mV
         *  25C = 0.580 mV
         *   0C = 0.424 mV
         * -25C = 0.268 mV
         * -40C = 0.174 mV
         */
        temperature = (lm60->read() * 3.3 - 0.424) / 0.00625;
    }

private:
    AnalogIn *lm60;
    Ticker trigger;

};
}

#endif //SHOECLEANER_LM60_H
