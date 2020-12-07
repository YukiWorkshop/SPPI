# SPPI
C++ wrapper of Linux's SPI userspace driver

## Features
- Easy to use
- Customizable chip selector, so you're not limited to the number of CS lines
- Built-in access lock, enables multiple process accessing one SPI controller 

## Usage
```cpp
using namespace YukiWorkshop;
```

Standard usage:
```cpp
SPPI s("/dev/spidev0.1");

uint8_t tx[16];
uint8_t rx[16];

s.transfer(tx, rx);

uint8_t data = s.transfer(0x00);
```

Custom chip selector:
```cpp
// The number after "." is irrelevant, just ensure it begins with spidev0 in this case
// This MAY conflict with SPI devices that are used by kernel 
SPPI s("/dev/spidev0.1", [&](bool cs_enable){
        if (cs_enable) {
                // Custom logic to select chip
        } else {
                // Custom logic to deselect chip
        }
});
```

For more usages, read the code.

## License
MIT