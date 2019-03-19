//
// Created by Forrest Jones on 2019-03-18.
//

#include <iostream>
#include "pulse_channel.h"

pulse_channel::pulse_channel(bool ones_complement) {
    this->ones_complement = ones_complement;
}

void pulse_channel::timer_complete() {
    sequence_counter = (uint8_t) ((sequence_counter + 1) % 8);
}

uint32_t pulse_channel::get_output() {
    if (target_timer_length > 0x7fff || get_timer_length() < 8)
        return 0;
    return duty_cycles[duty_cycle][sequence_counter] * get_volume();
}

void pulse_channel::half_frame_tick() {
    apu_channel::half_frame_tick();

    uint16_t change = timer_length >> shift;

    if (!negate) {
        target_timer_length = get_timer_length() + change;
    } else if (ones_complement) {
        target_timer_length = get_timer_length() - change - (uint16_t) 1;
    } else {
        target_timer_length = get_timer_length() - change;
    }

    if (divider == 0 && sweep_enable && !(target_timer_length > 0x7fff || get_timer_length() < 8)) {
        if (shift != 0) {
            apu_channel::set_timer_length_low((uint8_t) (target_timer_length & 0x00ff));
            apu_channel::set_timer_length_high((uint8_t) (target_timer_length >> 8));
        }
    }

    if (divider == 0 || reload_flag) {
        divider = divider_period;
        reload_flag = false;
    } else {
        divider--;
    }

    if (target_timer_length > 0x7fff || get_timer_length() < 8) {
        apu_channel::set_timer_length_low((uint8_t) (target_timer_length & 0x00ff));
        apu_channel::set_timer_length_high((uint8_t) (target_timer_length >> 8));
    }
}

void pulse_channel::set_timer_length_low(uint8_t length) {
    apu_channel::set_timer_length_low(length);
    timer_length = (uint16_t) ((timer_length & 0xff00) | length);
}

void pulse_channel::set_timer_length_high(uint8_t length) {
    apu_channel::set_timer_length_high(length);
    timer_length = (uint16_t) ((timer_length & 0x00ff) | (length << 8));
}

void pulse_channel::set_duty_cycle(uint8_t duty_cycle) {
    this->duty_cycle = duty_cycle;
}

void pulse_channel::set_sweep_enabled(bool enabled) {
    sweep_enable = enabled;
}

void pulse_channel::set_sweep_divider_period(uint8_t divider_period) {
    this->divider_period = divider_period;
}

void pulse_channel::set_sweep_negate_flag(bool negate) {
    this->negate = negate;
}

void pulse_channel::set_sweep_shift_count(uint8_t shift) {
    this->shift = shift;
    reload_flag = true;
}
