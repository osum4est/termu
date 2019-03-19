//
// Created by Forrest Jones on 2019-03-18.
//

#ifndef TERMU_PULSE_CHANNEL_H
#define TERMU_PULSE_CHANNEL_H


#include "apu_channel.h"
#include "apu_envelope_channel.h"

class pulse_channel : public apu_envelope_channel {
    volatile uint8_t duty_cycle;
    volatile uint8_t sequence_counter;
    const uint8_t duty_cycles[4][8] = {{1, 0, 0, 0, 0, 0, 0},
                                       {1, 1, 0, 0, 0, 0, 0},
                                       {1, 1, 1, 1, 0, 0, 0},
                                       {0, 0, 1, 1, 1, 1, 1}};

    bool ones_complement;

    uint16_t timer_length;
    uint16_t target_timer_length;

    bool sweep_enable;
    bool reload_flag;
    uint8_t divider;
    uint8_t divider_period;
    bool negate;
    uint8_t shift;

protected:
    void timer_complete() override;

public:
    explicit pulse_channel(bool ones_complement);

    uint32_t get_output() override;

    void half_frame_tick() override;

    void set_timer_length_low(uint8_t length) override;

    void set_timer_length_high(uint8_t length) override;

    void set_duty_cycle(uint8_t duty_cycle);

    void set_sweep_enabled(bool enabled);

    void set_sweep_divider_period(uint8_t divider_period);

    void set_sweep_negate_flag(bool negate);

    void set_sweep_shift_count(uint8_t shift);
};


#endif //TERMU_PULSE_CHANNEL_H
