//
// Created by osum4est on 3/4/19.
//

#include <curses.h>
#include <uiohook.h>
#include <iostream>
#include "keyboard_controller.h"

keyboard_controller *keyboard_controller::self = nullptr;

bool log(unsigned int i, const char *s, ...) {
    return false;
}

keyboard_controller::keyboard_controller(keyboard_controller::keymap map) : emu_controller() {
    this->map = map;
    self = this;
}

void keyboard_controller::read() {
}

void keyboard_controller::poll_input() {
    hook_set_dispatch_proc([](uiohook_event *const event) {
        if (event->type == EVENT_KEY_PRESSED) {
            if (event->data.keyboard.keycode == VC_X)
                keyboard_controller::self->a = true;
            else if (event->data.keyboard.keycode == VC_Z)
                keyboard_controller::self->b = true;
            else if (event->data.keyboard.keycode == VC_A)
                keyboard_controller::self->select = true;
            else if (event->data.keyboard.keycode == VC_S)
                keyboard_controller::self->start = true;
            else if (event->data.keyboard.keycode == VC_UP)
                keyboard_controller::self->dpad_up = true;
            else if (event->data.keyboard.keycode == VC_DOWN)
                keyboard_controller::self->dpad_down = true;
            else if (event->data.keyboard.keycode == VC_LEFT)
                keyboard_controller::self->dpad_left = true;
            else if (event->data.keyboard.keycode == VC_RIGHT)
                keyboard_controller::self->dpad_right = true;
        } else if (event->type == EVENT_KEY_RELEASED) {
            if (event->data.keyboard.keycode == VC_X)
                keyboard_controller::self->a = false;
            else if (event->data.keyboard.keycode == VC_Z)
                keyboard_controller::self->b = false;
            else if (event->data.keyboard.keycode == VC_A)
                keyboard_controller::self->select = false;
            else if (event->data.keyboard.keycode == VC_S)
                keyboard_controller::self->start = false;
            else if (event->data.keyboard.keycode == VC_UP)
                keyboard_controller::self->dpad_up = false;
            else if (event->data.keyboard.keycode == VC_DOWN)
                keyboard_controller::self->dpad_down = false;
            else if (event->data.keyboard.keycode == VC_LEFT)
                keyboard_controller::self->dpad_left = false;
            else if (event->data.keyboard.keycode == VC_RIGHT)
                keyboard_controller::self->dpad_right = false;
        }
    });
//    hook_set_logger_proc([](unsigned int i, const char *c, ...) -> bool {
//        return false;
//    });
    hook_set_logger_proc(log);
    hook_run();
}
