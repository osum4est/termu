//
// Created by osum4est on 3/4/19.
//

#ifndef TERMU_KEYBOARD_CONTROLLER_H
#define TERMU_KEYBOARD_CONTROLLER_H

#include <thread>
#include "../../../emu/core/input/emu_controller.h"

class keyboard_controller : public emu_controller {
public:
    struct keymap {
        int a;
        int b;
        int start;
        int select;
        int dpad_up;
        int dpad_down;
        int dpad_left;
        int dpad_right;
    };

private:
    keymap map;
    static keyboard_controller *polled_controller;
    static keymap *polled_map;

public:
    explicit keyboard_controller(keymap map);

    static void poll_input(keyboard_controller *controller);
};


#endif //TERMU_KEYBOARD_CONTROLLER_H
