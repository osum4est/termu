//
// Created by Forrest Jones on 2019-03-19.
//

#ifndef TERMU_APU_ENVELOPE_CHANNEL_H
#define TERMU_APU_ENVELOPE_CHANNEL_H


#include "apu_channel.h"

class apu_envelope_channel : public apu_channel {
    bool start;
    bool loop;
    volatile bool constant_volume;
    uint8_t divider;
    volatile uint8_t divider_period;
    volatile uint8_t decay_level;

protected:
    uint8_t get_volume();

public:
    apu_envelope_channel();

    void set_length_counter_halt(bool halt) override;

    void set_length_counter(uint16_t length) override;

    void quarter_frame_tick() override;

    void set_constant_volume_flag(bool constant_volume);

    void set_volume_divider_period(uint8_t divider_period);
};


#endif //TERMU_APU_ENVELOPE_CHANNEL_H
