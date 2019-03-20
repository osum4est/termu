//
// Created by Forrest Jones on 2019-03-11.
//

#ifndef TERMU_APU_H
#define TERMU_APU_H


#include <RtAudio.h>
#include "mem.h"
#include "sound/triangle_channel.h"
#include "interrupt_handler.h"
#include "sound/pulse_channel.h"
#include "sound/noise_channel.h"

class apu {
    ::mem *mem;
    ::interrupt_handler *interrupt_handler;

    uint32_t channels;
    uint32_t buffer_size;
    uint32_t sample_freq;
    double master_vol;

    RtAudio dac;
    RtAudio::StreamParameters stream_params;
    RtAudio::StreamOptions stream_options;

    // Registers
    uint8_t *pulse_1_envelope;
	uint8_t *pulse_1_sweep;
	uint8_t *pulse_1_timer_low;
	uint8_t *pulse_1_timer_high;
	uint8_t *pulse_2_envelope;
	uint8_t *pulse_2_sweep;
	uint8_t *pulse_2_timer_low;
	uint8_t *pulse_2_timer_high;
	uint8_t *triangle_linear_counter;
    uint8_t *triangle_timer_low;
    uint8_t *triangle_timer_high;
	uint8_t *noise_envelope;
	uint8_t *noise_mode;
	uint8_t *noise_length_counter;
    uint8_t *apu_status;
    uint8_t *apu_frame_counter;

    bool apu_cycle;
    bool frame_interrupt;
    bool frame_counter_mode;
    bool irq_inhibit;

    uint32_t frame_counter;

	double cycles_per_sample;

    volatile double *audio_buffer;
	volatile int audio_buffers;
	volatile int audio_buffer_size;

	volatile int audio_buffer_cycle_idx;
	volatile int audio_buffer_write_idx;
	volatile int audio_buffer_read_idx;
	volatile int buffer_gap;

    // Channels
    triangle_channel triangle;
    pulse_channel pulse_1 = pulse_channel(true);
	pulse_channel pulse_2 = pulse_channel(false);
	noise_channel noise;

    static int buffer_audio(void *output_buffer, void *input_buffer, uint32_t frames,
                            double time, RtAudioStreamStatus status, void *data);

    double mix_channels();

    void half_frame_tick();

    void quarter_frame_tick();

public:
    explicit apu(::mem *mem);

    void start();

    void set_interrupt_handler(::interrupt_handler *interrupt_handler);

    void cycle();

    void stop();

    void reg_handler(uint8_t &value, uint8_t new_value, bool write);
};


#endif //TERMU_APU_H
