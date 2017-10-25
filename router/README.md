# micro:net router

The code that actually runs on the micro:bit allowing it to act as a micro:net router.

## Developing

1. Follow the instructions outlined [here](https://lancaster-university.github.io/microbit-docs/offline-toolchains/) for setting up yotta on your platform. 

2. Make sure you've run both
```
yt target bbc-microbit-classic-gcc
```
and 
```
yt build
```
at least once in this directory in order to set the yotta target and build config.

3. Run 
  
```
fix-config.sh
```

This justs makes sure BLE is disabled since we're using radio.

4. Edit the last argument in flash.sh to point to the folder where your micro:bit is mounted.

Afterwards, you can compile and flash a microbit by simply running
```
yt build && flash.sh
```
