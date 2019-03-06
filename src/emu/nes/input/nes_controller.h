//
// Created by Forrest Jones on 2019-03-04.
//

#ifndef TERMU_NES_CONTROLLER_H
#define TERMU_NES_CONTROLLER_H

#include "nes_input_device.h"
#include "../../core/input/emu_controller.h"

class nes_controller : public nes_input_device {
private:
    emu_controller* controller;

    bool strobe = false;
    uint8_t shift = 0;
    uint8_t mem;

public:
    explicit nes_controller(emu_controller *controller);

    uint8_t &read() override;

    void write(uint8_t b) override;

private:
    uint8_t read_state();
};


#endif //TERMU_NES_CONTROLLER_H
