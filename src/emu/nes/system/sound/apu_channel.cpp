//
// Created by Forrest Jones on 2019-03-13.
//

#include "apu_channel.h"

apu_channel::apu_channel() {
    halt = false;
    enabled = false;
    timer = 0;
    timer_length = 0;
    timer_ticks = 0;
    timer_tick_length = 1;
}

void apu_channel::set_timer_tick_length(uint8_t ticks) {
    timer_tick_length = ticks;
}

void apu_channel::cpu_tick() {
    timer_ticks++;
    if (timer_ticks >= timer_tick_length) {
        if (timer == 0) {
            timer_tick();
            timer = timer_length;
        } else {
            timer--;
        }

        timer_ticks = 0;
    }
}

void apu_channel::half_frame_tick() {
    if (!halt && length_counter > 0)
        length_counter--;
}

void apu_channel::quarter_frame_tick() {

}

void apu_channel::set_length_counter(uint16_t length) {
    length_counter = length;
}

void apu_channel::set_length_counter_halt(bool halt) {
    this->halt = halt;
}

void apu_channel::set_length_counter_enabled(bool enabled) {
    this->enabled = enabled;

    if (!enabled)
        length_counter = 0;
}

void apu_channel::set_timer_length_low(uint8_t length) {
    timer_length = (uint16_t) ((timer_length & 0xff00) | length);
}

void apu_channel::set_timer_length_high(uint8_t length) {
    timer_length = (uint16_t) ((timer_length & 0x00ff) | (length << 8));
}

uint16_t apu_channel::get_length_counter() {
    return length_counter;
}
