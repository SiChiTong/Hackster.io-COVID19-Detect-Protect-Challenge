//
// Heater Controller
// Controls heater sw
// Created by Tatsuya Iwai on 2020/05/17.
// Copyright (c) 2020 greysound.com. All rights reserved.
//

#ifndef SHOECLEANER_HEATER_CONTROLLER_H
#define SHOECLEANER_HEATER_CONTROLLER_H

#define PID_KP 10.0f
#define PID_KI 0.2f
#define PID_KD 10.0f
#define SAMPLE_BUFFER_LENGTH 8 // 1-255

#include <mbed.h>
#include "PIDController.h"

namespace greysound {

static const float WATER_LEVEL_UPDATE_PERIOD_SEC = 0.5f;
static const float WATER_THRESHOLD = 0.1f;

static const float HEATER_UPDATE_PERIOD_SEC = 1.0f;
static const float PID_TARGET_TEMP_CELSIUS = 60.0f;

enum WATER_LEVEL {
    WATER_LEVEL_NG = 0,
    WATER_LEVEL_OK = 1
} ;

enum AC_OUT {
    ENABLE_AC_OUT = 1,
    DISABLE_AC_OUT = 0
} ;

class HeaterController {

public:
    PIDController *pidController; //TODO: Move to private after debug
    double pidValue; //TODO: Move to private after debug
    double floatSensorAverage;
    uint16_t acPulseCounter;
    uint8_t maxCountAcEnable; // 0-100
    static const uint8_t SAMPLING_AC_COUNT = 100;

    /**
     * Constructor
     * @param triggerPin
     * @param triggerPin
     * @param echoPin
     */
    HeaterController(PinName acPulseInput, PinName tempSensor, PinName floatSensorIn, PinName enableAcOutput)
    {
        /**
         * Init vars and event handler for AC pluse-in
         */
        acPulseCounter = 0;
        maxCountAcEnable = 0;
        acPulseIn = new InterruptIn(acPulseInput);
        enableAcOut = new DigitalOut(enableAcOutput);

        /**
         * Init Float Switch (water level sensor)
         */
        floatBufferIndex = 0;
        // init buffer with lowest value
        for (uint8_t i=0; i<SAMPLE_BUFFER_LENGTH; i++) {
            floatValuesBuffer[i] = WATER_THRESHOLD;
        }
        floatSensorAverage = WATER_THRESHOLD;
        floatSensor = new AnalogIn(floatSensorIn);

        /**
         * Start periodic trigger for reading water level
         */
        updateFloatTicker.attach(callback(this, &HeaterController::readFloatValue), WATER_LEVEL_UPDATE_PERIOD_SEC);

        /**
         * Init Water Temperature Sensor (LM60)
         */
        lm60 = new LM60(tempSensor);
        lm60->start();

        /**
         * Init PID Control
         */
        pidValue = 0.0f;
        pidController = new PIDController(PID_KP, PID_KI, PID_KD);
        pidController->init(PID_TARGET_TEMP_CELSIUS, (float) getCurrentTemp());
    }

    /**
     * destructor
     */
    ~HeaterController()
    {
        // stop before delete
        stop();
        enableAcOut->write(DISABLE_AC_OUT);
        updateFloatTicker.detach();
        lm60->stop();
        acPulseIn->fall(NULL);

        delete(enableAcOut);
        delete(lm60);
        delete(floatSensor);
        delete(acPulseIn);
    }

    /**
     * Start Heater Control
     */
    void start()
    {
        // stop first
        stop();

        // start watching AC pulse-in
        maxCountAcEnable = 0;
        acPulseIn->fall(callback(this, &HeaterController::triggerAcPulseIn));

        // set periodic ticker to calculate PID value
        updateHeaterTicker.attach(callback(this, &HeaterController::updateMaxCountAcEnable), HEATER_UPDATE_PERIOD_SEC);
    }

    /**
     * Stop Heater Control
     */
    void stop()
    {
        // stop PID calculation
        updateHeaterTicker.detach();

        // stop watching AC pulse-in
        acPulseIn->fall(NULL);
    }

    /**
     * enable AC out for certain time period
     */
    void triggerAcPulseIn()
    {
        // exception: disable if water level is not ok
        if(getCurrentWaterState() == WATER_LEVEL_NG)
        {
            enableAcOut->write(DISABLE_AC_OUT);
            return;
        }

        // enable AC out while counter value is less(or equal) than max value
        if (++acPulseCounter <= maxCountAcEnable)
        {
            enableAcOut->write(ENABLE_AC_OUT);
        } else {
            enableAcOut->write(DISABLE_AC_OUT);
        }

        // clear counter if exceeds max sampling number
        acPulseCounter = acPulseCounter % SAMPLING_AC_COUNT;
    }

    double getCurrentTemp()
    {
        return lm60->temperature;
    }

    WATER_LEVEL getCurrentWaterState() const
    {
        if(floatSensorAverage >= WATER_THRESHOLD) {
            return WATER_LEVEL_OK;
        }
        return WATER_LEVEL_NG;
    }

private:
    // AC Pulse
    InterruptIn *acPulseIn;

    // Temp Sensor
    LM60 *lm60;

    // Water Level Sensor (Magnet SW)
    AnalogIn *floatSensor;
    Ticker updateFloatTicker;
    double floatValuesBuffer[SAMPLE_BUFFER_LENGTH];
    uint8_t floatBufferIndex;

    // PID Control
    DigitalOut *enableAcOut;
    Ticker updateHeaterTicker;

    /**
     * Read analog value based on water level
     */
    void readFloatValue()
    {
        // read and store float value
        floatValuesBuffer[floatBufferIndex++] = floatSensor->read();

        // reset index if exceeds buffer length
        floatBufferIndex = floatBufferIndex % SAMPLE_BUFFER_LENGTH;

        // calc average value and update water level state
        updateWaterLevel();
    }

    /**
     * Calc average value of water level sampled
     */
    void updateWaterLevel()
    {
        // clear
        floatSensorAverage = 0.0f;

        // sum values
        for (uint8_t i=0; i<SAMPLE_BUFFER_LENGTH; i++) {
            floatSensorAverage += floatValuesBuffer[i];
        }

        // calc average
        floatSensorAverage = floatSensorAverage / SAMPLE_BUFFER_LENGTH; // 0-1.0f
    }

    /**
     * update maximum number of AC pulse enabling AC out based on PID estimation
     */
    void updateMaxCountAcEnable()
    {
        pidController->updatePID((float) getCurrentTemp());
        pidValue = pidController->PID;

        // keep pidValue between 1.0-0.0f
        if (pidValue > 1.0f) {
            pidValue = 1.0f;
        }
        if (pidValue < 0) {
            pidValue = 0;
        }

        // update current AC out enable duration
        maxCountAcEnable = int(pidValue * 100);
    }
};
}

#endif //SHOECLEANER_HEATER_CONTROLLER_H
