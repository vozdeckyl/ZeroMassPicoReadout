# Single Channel
Detects pulses on a single channel.

## Compiling Code
Compile the code into a `build/` directory using cmake:
```
mkdir build
cd build
cmake ../ -DPICO_SDK_PATH=../../pico-sdk/
make -j10
```

## Connecting the board
The pre-amplifier output has to be connected to the ADC0 channel (pin 31) and the ground has to be connected to GND (pin 28).