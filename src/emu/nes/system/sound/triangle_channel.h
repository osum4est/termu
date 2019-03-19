//
// Created by Forrest Jones on 2019-03-13.
//

#ifndef TERMU_TRIANGLE_CHANNEL_H
#define TERMU_TRIANGLE_CHANNEL_H


#include "apu_channel.h"

// TODO: Fix DK: notes are legato
class triangle_channel : public apu_channel {
    volatile uint8_t sequence_counter;
    const uint8_t sequence[32] = {0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
                                  0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

    uint8_t linear_counter;
    uint8_t linear_counter_reload;
    bool linear_counter_reload_flag;
    bool control_flag;

protected:
    void timer_complete() override;

public:
    uint32_t get_output() override;

    void quarter_frame_tick() override;

    void set_linear_counter_reload(uint8_t reload);

    void set_length_counter(uint16_t length) override;

    void set_length_counter_halt(bool halt) override;
};


#endif //TERMU_TRIANGLE_CHANNEL_H
