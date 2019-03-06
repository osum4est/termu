//
// Created by osum4est on 3/4/19.
//

#ifndef TERMU_CONTROLLER_H
#define TERMU_CONTROLLER_H

class emu_controller {
public:
    bool a = false;
    bool b = false;
    bool start = false;
    bool select = false;
    bool dpad_up = false;
    bool dpad_down = false;
    bool dpad_left = false;
    bool dpad_right = false;

    void reset() {
        a = false;
        b = false;
        start = false;
        select = false;
        dpad_up = false;
        dpad_down = false;
        dpad_left = false;
        dpad_right = false;
    }

    virtual void read() = 0;
};

#endif //TERMU_CONTROLLER_H
