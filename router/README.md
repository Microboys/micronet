# Micro:net

## Instructions
1) Make sure you've run 
```
yt target bbc-microbit-classic-gcc
```
at least once in order to set the yotta target.

2) Disable BLE by setting
  
```
#define MICROBIT_BLE_ENABLED 0
```
in yotta_modules/microbit-dal/inc/core/MicroBitConfig.h

Alternatively, run fix-config.sh, which replaces the config file with one that has BLE disabled.

3) Edit the last argument in flash.sh to point to the folder where your micro:bit is mounted.


Afterwards, you can compile and flash a microbit by running
```
yt build && flash.sh
```
