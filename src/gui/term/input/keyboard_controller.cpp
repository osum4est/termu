//
// Created by osum4est on 3/4/19.
//

#include <curses.h>
#include <uiohook.h>
#include <iostream>
#include "keyboard_controller.h"

keyboard_controller *keyboard_controller::polled_controller = nullptr;
keyboard_controller::keymap *keyboard_controller::polled_map = nullptr;

keyboard_controller::keyboard_controller(keyboard_controller::keymap map) : emu_controller() {
    this->map = map;
}

/**
 * C++ doesn't support variadic lambdas yet so I gotta make a whole function for this...
 */
bool log(unsigned int i, const char *s, ...) {
    return true;
}

void keyboard_controller::poll_input(keyboard_controller *controller) {
    // TODO: Support multiple controllers/players
    keyboard_controller::polled_controller = controller;
    keyboard_controller::polled_map = &controller->map;

    hook_set_dispatch_proc([](uiohook_event *const event) {
        bool pressed;
        if (event->type == EVENT_KEY_PRESSED)
            pressed = true;
        else if (event->type == EVENT_KEY_RELEASED)
            pressed = false;
        else
            return;

        keyboard_controller *c = keyboard_controller::polled_controller;
        keymap *m = keyboard_controller::polled_map;
		auto key = event->data.keyboard.keycode;

        if (c == nullptr || m == nullptr)
            return;

		if (key == m->a)
			c->a = pressed;
		else if (key == m->b)
			c->b = pressed;
		else if (key == m->select)
			c->select = pressed;
		else if (key == m->start)
			c->start = pressed;
		else if (key == m->dpad_up)
			c->dpad_up = pressed;
		else if (key == m->dpad_down)
			c->dpad_down = pressed;
		else if (key == m->dpad_left)
			c->dpad_left = pressed;
		else if (key == m->dpad_right)
			c->dpad_right = pressed;
		else if (key == m->dpad_right)
			c->dpad_right = pressed;
		else if (key == m->quit)
			hook_stop();
    });

    hook_set_logger_proc(log);
    hook_run();
}

