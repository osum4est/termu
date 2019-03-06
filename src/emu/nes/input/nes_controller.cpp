//
// Created by Forrest Jones on 2019-03-04.
//

#include "nes_controller.h"
#include "../../../utils/utils.h"

nes_controller::nes_controller(emu_controller *controller) {
    this->controller = controller;
}

uint8_t &nes_controller::read() {
    if (strobe)
        shift = 0;

    mem = (uint8_t) (0x40 | ((uint8_t)(read_state() << shift) >> 7));
    shift++;
    return mem;
}

void nes_controller::write(uint8_t b) {
    strobe = b != 0;
}

uint8_t nes_controller::read_state() {
    controller->read();
    return (uint8_t) ((controller->a << 7) |
                      (controller->b << 6) |
                      (controller->select << 5) |
                      (controller->start << 4) |
                      (controller->dpad_up << 3) |
                      (controller->dpad_down << 2) |
                      (controller->dpad_left << 1) |
                      (controller->dpad_right));

}