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
    uint8_t timer_ticks;
    uint8_t timer_tick_length;

protected:
    bool halt;
    uint16_t length_counter;

    void set_timer_tick_length(uint8_t ticks);

    virtual void timer_tick() = 0;

public:
    apu_channel();

    void cpu_tick();

    virtual void half_frame_tick();

    virtual void quarter_frame_tick();

    virtual uint32_t get_output() = 0;

    virtual void set_length_counter(uint16_t length);

    virtual void set_length_counter_halt(bool halt);

    void set_length_counter_enabled(bool enabled);

    void set_timer_length_low(uint8_t length);

    void set_timer_length_high(uint8_t length);

    uint16_t get_length_counter();
};


#endif //TERMU_APU_CHANNEL_H
