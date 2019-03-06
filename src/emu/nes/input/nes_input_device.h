//
// Created by Forrest Jones on 2019-03-04.
//

#ifndef TERMU_NES_INPUT_DEVICE
#define TERMU_NES_INPUT_DEVICE

#include <cstdint>

class nes_input_device {
public:
    virtual ~nes_input_device() = default;
    virtual uint8_t &read() = 0;
    virtual void write(uint8_t b) = 0;
};

#endif //TERMU_NES_INPUT_DEVICE_H
