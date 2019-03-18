//
// Created by Forrest Jones on 2019-03-13.
//

#include "triangle_channel.h"

triangle_channel::triangle_channel() {
    set_timer_tick_length(1);
}

void triangle_channel::timer_tick() {
    if (linear_counter > 0 && length_counter > 0)
        sequence_counter = (uint8_t) ((sequence_counter + 1) % 32);
}

void triangle_channel::quarter_frame_tick() {
    apu_channel::quarter_frame_tick();

    if (linear_counter_reload_flag)
        linear_counter = linear_counter_reload;
    else if (linear_counter > 0)
        linear_counter--;

    if (!control_flag)
        linear_counter_reload_flag = false;
}

uint32_t triangle_channel::get_output() {
    return sequence[sequence_counter];
}

void triangle_channel::set_linear_counter_reload(uint8_t reload) {
    linear_counter_reload = reload;
}

void triangle_channel::set_length_counter(uint16_t length) {
    apu_channel::set_length_counter(length);
    linear_counter_reload_flag = true;
}

void triangle_channel::set_length_counter_halt(bool halt) {
    apu_channel::set_length_counter_halt(halt);
    control_flag = halt;
}
