# PCA9546A I2C Bus Switch Arduino Control Library
## Basic Usage
Creating a device 
```
#include <PCA9546A.h>

//Create a device with the default address 
PCA9546A busSwitcher();

//Create a device with the supplied address
PCA9546A busSwitcher(0x71);

//Create a device based on the hardware pin states,
//A0 is HIGH, A1 is HIGH, A2 is LOW
PCA9546A busSwitcher(HIGH, HIGH, LOW);

//Create a device with an alternate TwoWire instance
PCA9546A busSwitcher(Wire2, 0x70);
```

Selecting a channel to communicate 
```
//Activate I2C channel 0 only
busSwitcher.selectChannelExclusive(0);
//TODO: Transmit to upstream device
busSwitcher.disableChannel(0);

//Activate I2C channel 0 and 2 and transmit on both
busSwitcher.enableChannel(0);
busSwitcher.enableChannel(2);
//TODO: Transmit to upstream device
busSwitcher.disableAll();
```