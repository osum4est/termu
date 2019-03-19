//
// Created by Forrest Jones on 2019-03-13.
//

#ifndef TERMU_APU_CHANNEL_H
#define TERMU_APU_CHANNEL_H


#include <cstdint>

class apu_channel {
    bool enabled;

    uint16_t timer;
    uint16_t timer_length;

    uint8_t length_table[32] = {10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
                                12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

protected:
    bool halt;
    uint16_t length_counter;

    uint16_t get_timer_length();

    virtual void timer_complete() = 0;

public:
    apu_channel();

    void timer_tick();

    virtual void half_frame_tick();

    virtual void quarter_frame_tick();

    virtual uint32_t get_output() = 0;

    virtual void set_length_counter(uint16_t length);

    virtual void set_length_counter_halt(bool halt);

    void set_length_counter_enabled(bool enabled);

    virtual void set_timer_length_low(uint8_t length);

    virtual void set_timer_length_high(uint8_t length);

    uint16_t get_length_counter();
};


#endif //TERMU_APU_CHANNEL_H
