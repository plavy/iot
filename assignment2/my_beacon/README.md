# My Beacon

BLE beacon is started that is visible in nRF Connect app.

Temperature is read from HTS sensor, printed out and sent by the beacon as `Temperature Measurement`.

## Initialization

Ensure env variable `ZEPHYR_HOME` is set and then run:
```bash
mkdir build && cd build && cmake .. && west build
```

## Flashing to the board

To flash it to the board thingy52_nrf52832 run:
```bash
west flash
```

## Read RTT

In one terminal run:
```bash
JLinkExe -if SWD
```
In the opened shell, type `connect` and press enter to accept all the default values.

In a new terminal run:
```
JLinkRTTClient
```
You should be able to see the logs.
