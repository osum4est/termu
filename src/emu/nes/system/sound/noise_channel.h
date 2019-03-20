//
// Created by Forrest Jones on 2019-03-19.
//

#ifndef TERMU_NOISE_CHANNEL_H
#define TERMU_NOISE_CHANNEL_H


#include "apu_envelope_channel.h"

class noise_channel : public apu_envelope_channel {
    // For NTSC
    uint16_t periods[16] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};

    bool mode;
    uint16_t shift;

protected:
    void timer_complete() override;

public:
    noise_channel();

    uint32_t get_output() override;

    void set_mode(bool mode);

    void set_period(uint8_t period);
};


#endif //TERMU_NOISE_CHANNEL_H
