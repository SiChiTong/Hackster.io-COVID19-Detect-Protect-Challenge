# The COVID-19 Detect & Protect Challenge

![EyeCatch](https://github.com/iwaita2ya/ImageStore/blob/master/the-COVID-19-detect-and-protect-challenge-cover.jpg)

This repository holds source code of the COVID-19 Detect & Protect Challenge contest
held by [hackster.io](https://www.hackster.io/contests/UNDPCOVID19).

This program requires EA LPC11u35 or compatible board with some modules.
You can find complete list of hardware with schematic diagram at my project submission page, 
but submission page may not be available until winners are announced.

This repository will be left as-is until submission closed and winners are announced by hackster.io. 
I will make changes/update for future.

## Things you need
This programs requires [EA LPC11u35](https://www.embeddedartists.com/products/lpc11u35-quickstart/)
which communicates with:
* [Ultrasonic Sensor HC-SR04](https://www.sparkfun.com/products/15569)
* [Analog Temperature Sensor LM60](https://www.ti.com/product/LM60)
* [Dual H-bridge Motor Driver L298N](https://www.smart-prototyping.com/L298N-Dual-H-bridge-Motor-Driver-Board)
* Generic [Water Level Sensor](https://www.walmart.com/ip/5Pcs-Water-Level-Sensor-Float-Switch-ZP4510-for-Aquarium-Pump-Control/45587105?wmlspartner=wlpa&selectedSellerId=571)
* [Photocoupler LTP627](https://toshiba.semicon-storage.com/us/semiconductor/product/optoelectronics/detail.TLP627-4.html)
* [IRED & Photo Transistor LTP561J](https://toshiba.semicon-storage.com/us/semiconductor/product/optoelectronics/detail.TLP561J.html)

![Schema](https://github.com/iwaita2ya/ImageStore/blob/master/the-COVID-19-detect-and-protect-challenge-schematic.png)
Please see [here](https://drive.google.com/file/d/18t1TlQfpdtOFd1a6cuX6zBULBZKXoopP/view?usp=sharing) for the complete schematic circuit diagram.

## Installation

You need to configure [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
and [PlatformIO Core](https://docs.platformio.org/en/latest/core.html) for build.

### Clone & Build
```bash
$ git clone https://github.com/iwaita2ya/Hackster.io-COVID19-Detect-Protect-Challenge
$ cd ./Hackster.io-COVID19-Detect-Protect-Challenge
$ platformio run
```

### Install
After the build completed, you'll see binary file "firmware.bin" under .pio/build/lpc11u35/ directory located in the project directory.

Then connect LPC11u35 onto your PC and copy the binary file.

```bash
cp -p .pio/build/lpc11u35/firmware.bin [/path/to/LPC11u35]
```

Under Windows10 environment, you may need to install serial port driver before connect LPC11u35.
Please see [here](https://os.mbed.com/handbook/Windows-serial-configuration) for more detail.

## Development Environment

This program has been developed under following environment:
* Ubuntu ([Pop!_OS](https://system76.com/pop)) 20.04 LTS x86_64
* [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
* [PlatformIO Core](https://platformio.org/)
* [Jetbrains CLion IDE](https://www.jetbrains.com/clion/)
