//
// Created by Tatsuya Iwai on 2/29/20.
// Copyright (c) 2020 greysound.com. All rights reserved.
//

#define DEBUG

#define FOOT_DETECT_DIST_CM 18
#define WATER_MAX_TEMPERATURE 60.0f

#include <mbed.h>
#include "HCSR04.h"
#include "MotorDriver.h"
#include "HeaterController.h"

using namespace greysound;

/**
 * flags / vars
 */
uint8_t shouldLoop;
double currentDistance;
uint8_t rollerStatus;

/**
 * Serial Port
 */
RawSerial *serial;

#ifdef DEBUG
#define DEBUG_PRINT(fmt) serial->printf(fmt)
#define DEBUG_PRINTF(fmt, ...) serial->printf(fmt, __VA_ARGS__)
#define DEBUG_PUTC(x) serial->putc(x)
#else
#define DEBUG_PRINT(fmt)
#define DEBUG_PRINTF(fmt, ...)
#define DEBUG_PUTC(x)
#endif

/**
 * HC-SR04 Distance Sensor
 */
HCSR04 *hcsr04;

/**
 * Motor Driver
 */
MotorDriver *motorDriver;

/**
 * Heater Controller
 */
HeaterController *heaterController;

/**
 * Digital Out
 */
DigitalOut *ledVital; // Vital Indicator
DigitalOut *ledWaterError; // indicates when water level too low
DigitalOut *ledHeaterError; // indicates when heater is too high

/**
 * Statuses
 */
enum ROLLER_STATUS {
    ROLLER_STOP = 0,
    ROLLER_FORWARD = 1
};

enum LED_STATUS {
    LED_OFF = 0,
    LED_ON = 1
};

// Function Prototypes --------------------------------------------------------
void updateRollerStatus();

// Main  ----------------------------------------------------------------------
int main()
{
    /**
     * Init Flags/Vars
     */
    shouldLoop = 1;
    rollerStatus = ROLLER_STOP;

    /**
     * Init Serial
     */
    serial = new RawSerial(P0_19, P0_18); // tx, rx
    serial->baud(115200);

    /**
     * Init Distance Sensor (HCSR04)
     */
    DEBUG_PRINT("Init Distance Sensor\r\n");
    hcsr04 = new HCSR04(P0_13, P0_12);
    hcsr04->start();

    /**
     * Init Motor Driver
     */
    DEBUG_PRINT("Init Motor Driver\r\n");
    motorDriver = new MotorDriver(P0_22);

    /**
     * Init Heater Controller
     */
    DEBUG_PRINT("Init Heater Controller.\r\n");
    heaterController = new HeaterController(P0_16, P0_11, P0_15, P0_23);
    heaterController->start();

    /**
     * Init LED
     */
    DEBUG_PRINT("Init LEDs.\r\n");
    // vital indicator
    ledVital = new DigitalOut(P0_7);
    ledVital->write(0); // set led off
    // heater error indicator
    ledHeaterError = new DigitalOut(P0_8);
    ledVital->write(0); // set led off
    // water level error indicator
    ledWaterError = new DigitalOut(P0_9);
    ledWaterError->write(0); // set led off

    /**
     * Main Loop
     */
    DEBUG_PRINT("Start looping...\r\n");
    //uint32_t timeCount = 0; //MEMO: for debug
    while(shouldLoop == 1)
    {
        // update roller status
        //DEBUG_PRINTF("Distance: %dcm\r\n", int(currentDistance));
        updateRollerStatus();

        // Display Heater Temperature
        //DEBUG_PRINTF("Heater Temp: %d℃\r\n", int(heaterController->getCurrentTemp()));
        if(heaterController->getCurrentTemp() > WATER_MAX_TEMPERATURE) {
            //DEBUG_PRINT("[WARN] Water is TOO HOT!!!\r\n");
            ledHeaterError->write(LED_ON);
        } else {
            ledHeaterError->write(LED_OFF);
        }

        // Monitor Water State
        //DEBUG_PRINTF("Water Value(x1000):%d\r\n", (int)(heaterController->floatSensorAverage * 1000));
        if(heaterController->getCurrentWaterState() == WATER_LEVEL_NG) {
            //DEBUG_PRINT("[WARN] Water Level is TOO LOW!!!\r\n");
            ledWaterError->write(LED_ON);
        } else {
            ledWaterError->write(LED_OFF);
        }

        /*
        //MEMO: Please increase wait value when you enable debug output. Otherwise it's too fast!
        DEBUG_PRINTF("%d, %d, %d, %d, %d, %d, %d\r\n"
            , (int)timeCount++
            , (int)heaterController->getCurrentTemp()
            , (int)heaterController->pidController->P
            , (int)heaterController->pidController->I
            , (int)heaterController->pidController->D
            , (int)(heaterController->pidValue * 100)
            , heaterController->maxCountAcEnable
            );
        */

        // Flip LED value
        ledVital->write(!(ledVital->read()));
        wait(0.1f);
    }
}

// Functions ---------------------------------------------------------------------
void updateRollerStatus()
{
    currentDistance = hcsr04->distance;
    // shoe detected - start roller
    if (currentDistance < FOOT_DETECT_DIST_CM) {
        // start roller forwarding
        if (rollerStatus == ROLLER_STOP) {
            rollerStatus = ROLLER_FORWARD;
            motorDriver->forwardA();
        }
    } else { // shoe not detected
        // stop roller
        if (rollerStatus != ROLLER_STOP) {
            rollerStatus = ROLLER_STOP;
            motorDriver->stopA();
        }
    }

}

