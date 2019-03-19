//
// Created by Forrest Jones on 2019-03-19.
//

#include "apu_envelope_channel.h"

apu_envelope_channel::apu_envelope_channel() {
    start = true;
}

uint8_t apu_envelope_channel::get_volume() {
    if (constant_volume)
        return divider_period;
    return decay_level;
}

void apu_envelope_channel::set_length_counter_halt(bool halt) {
    apu_channel::set_length_counter_halt(halt);
    loop = halt;
}

void apu_envelope_channel::set_length_counter(uint16_t length) {
    apu_channel::set_length_counter(length);
    start = true;
}

void apu_envelope_channel::quarter_frame_tick() {
    apu_channel::quarter_frame_tick();

    if (start) {
        start = false;
        decay_level = 15;
        divider = divider_period;
    } else {
        if (divider == 0) {
            divider = divider_period;
            if (decay_level > 0)
                decay_level--;
            else if (loop)
                decay_level = 15;
        } else {
            divider--;
        }
    }
}

void apu_envelope_channel::set_constant_volume_flag(bool constant_volume) {
    this->constant_volume = constant_volume;
}

void apu_envelope_channel::set_volume_divider_period(uint8_t divider_period) {
    this->divider_period = divider_period;
}
