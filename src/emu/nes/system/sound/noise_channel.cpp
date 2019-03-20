//
// Created by Forrest Jones on 2019-03-19.
//

#include <iostream>
#include "noise_channel.h"

noise_channel::noise_channel() {
    shift = 1;
}

void noise_channel::timer_complete() {
    bool feedback = (bool) ((shift & 0x01) ^ ((shift >> (mode ? 6 : 1)) & 0x01));
    shift >>= 1;
    shift |= feedback << 14;
}

uint32_t noise_channel::get_output() {
    if ((shift & 0x01) || length_counter == 0)
        return 0;
//    std::cout << (shift & 0x01) << " and " << length_counter << std::endl;
//return 0xf;
    return get_volume();
}

void noise_channel::set_mode(bool mode) {
    this->mode = mode;
}

void noise_channel::set_period(uint8_t period) {
    set_timer_length_low((uint8_t) (periods[period] & 0x00ff));
    set_timer_length_high((uint8_t) (periods[period] >> 8));
}
