//
// Created by Forrest Jones on 2019-03-11.
//

#include "apu.h"
#include "../../../utils/utils.h"
#include "../nes_emulator.h"

#include <thread>
#include <cmath>

apu::apu(::mem *mem) {
    this->mem = mem;
}

void apu::start() {
    if (dac.getDeviceCount() < 1)
        throw emu_exception("Could not find any audio devices.");

    pulse_1_envelope = &mem->get_cpu(0x4000);
    pulse_1_sweep = &mem->get_cpu(0x4001);
    pulse_1_timer_low = &mem->get_cpu(0x4002);
    pulse_1_timer_high = &mem->get_cpu(0x4003);
    pulse_2_envelope = &mem->get_cpu(0x4004);
    pulse_2_sweep = &mem->get_cpu(0x4005);
    pulse_2_timer_low = &mem->get_cpu(0x4006);
    pulse_2_timer_high = &mem->get_cpu(0x4007);
    triangle_linear_counter = &mem->get_cpu(0x4008);
    triangle_timer_low = &mem->get_cpu(0x400a);
    triangle_timer_high = &mem->get_cpu(0x400b);
    noise_envelope = &mem->get_cpu(0x400c);
    noise_mode = &mem->get_cpu(0x400e);
    noise_length_counter = &mem->get_cpu(0x400f);
    apu_status = &mem->get_cpu(0x4015);
    apu_frame_counter = &mem->get_cpu(0x4017);

    mem->set_apu_reg_handler(
            std::bind(&apu::reg_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    apu_cycle = false;

    triangle.set_length_counter_enabled(false);
    pulse_1.set_length_counter_enabled(false);
    pulse_2.set_length_counter_enabled(false);
    noise.set_length_counter_enabled(false);

    channels = 2;
    buffer_size = 2048;
    frame_counter = 0;
    sample_freq = 48000;
    master_vol = .5;

    stream_params.deviceId = dac.getDefaultOutputDevice();
    stream_params.firstChannel = 0;
    stream_params.nChannels = channels;

    stream_options.flags = RTAUDIO_SCHEDULE_REALTIME;

    cycles_per_sample = (double) nes_emulator::CLOCK_RATE / sample_freq;

    audio_buffers = 5;
    audio_buffer_size = buffer_size * audio_buffers;
    audio_buffer = new double[audio_buffer_size];

    audio_buffer_cycle_idx = 0;
    audio_buffer_write_idx = 0;
    audio_buffer_read_idx = 0;
    buffer_gap = audio_buffers - 1;

    dac.openStream(&stream_params, nullptr, RTAUDIO_FLOAT64, sample_freq, &buffer_size, &buffer_audio, this,
                   &stream_options);
    dac.startStream();
}

void apu::cycle() {
    while (buffer_gap >= audio_buffers - 1) {}

    triangle.timer_tick();

    if (apu_cycle) {
        pulse_1.timer_tick();
        pulse_2.timer_tick();
        noise.timer_tick();

        frame_counter++;
        if (frame_counter == 3728) {
            quarter_frame_tick();
        } else if (frame_counter == 7456) {
            quarter_frame_tick();
            half_frame_tick();
        } else if (frame_counter == 11185) {
            quarter_frame_tick();
        } else if (frame_counter == 14914 && !frame_counter_mode) {
            quarter_frame_tick();
            half_frame_tick();
            if (!irq_inhibit) {
                frame_interrupt = true;
                interrupt_handler->set_irq();
            }
            frame_counter = 0;
        } else if (frame_counter == 18640 && frame_counter_mode) {
            quarter_frame_tick();
            half_frame_tick();
            frame_counter = 0;
        }
    }
    apu_cycle = !apu_cycle;

    if (audio_buffer_cycle_idx >= (int) cycles_per_sample) {
        audio_buffer_cycle_idx = 0;
        audio_buffer[audio_buffer_write_idx] = mix_channels();
        if (audio_buffer_write_idx == audio_buffer_size - 1) {
            audio_buffer_write_idx = 0;
        } else {
            audio_buffer_write_idx++;
        }

        if (audio_buffer_write_idx % buffer_size == 0)
            buffer_gap++;
    } else {
        audio_buffer_cycle_idx++;
    }
}

void apu::stop() {
    dac.stopStream();
}

int apu::buffer_audio(void *output_buffer, void *input_buffer, uint32_t frames,
                      double time, RtAudioStreamStatus status, void *data) {
    if (status)
        throw emu_exception("Audio stream underflow.");

    apu *self = static_cast<apu *>(data);
    double *buffer = static_cast<double *>(output_buffer);

    for (int i = 0; i < frames; i++) {
        for (int j = 0; j < self->channels; j++, buffer++) {
            *buffer = self->audio_buffer[self->audio_buffer_read_idx];
        }

        if (self->audio_buffer_read_idx == self->audio_buffer_size - 1) {
            self->audio_buffer_read_idx = 0;
        } else {
            self->audio_buffer_read_idx++;
        }
    }

    self->buffer_gap--;

    return 0;
}

double apu::mix_channels() {
    // TODO: Mix using NES non-linear method
    double out = 0;

    out += ((double) triangle.get_output() - 8) / 8;
    out += ((double) pulse_1.get_output() - 8) / 8;
    out += ((double) pulse_2.get_output() - 8) / 8;
    out += ((double) noise.get_output() - 8) / 8;
    out /= 4;

    return out * master_vol;
}

void apu::reg_handler(uint8_t &value, uint8_t new_value, bool write) {
    if (&value == pulse_1_envelope && write) {
        pulse_1.set_duty_cycle(new_value >> 6);
        pulse_1.set_length_counter_halt((bool) ((new_value >> 5) & 0x01));
        pulse_1.set_constant_volume_flag((bool) ((new_value >> 4) & 0x01));
        pulse_1.set_volume_divider_period((uint8_t(new_value & 0x0f)));
    } else if (&value == pulse_1_sweep && write) {
        pulse_1.set_sweep_enabled((bool) (new_value >> 7));
        pulse_1.set_sweep_divider_period((uint8_t)((new_value >> 4) & 0x07));
        pulse_1.set_sweep_negate_flag((bool) ((new_value >> 3) & 0x01));
        pulse_1.set_sweep_shift_count((uint8_t)(new_value & 0x07));
    } else if (&value == pulse_1_timer_low && write) {
        pulse_1.set_timer_length_low(new_value);
    } else if (&value == pulse_1_timer_high && write) {
        pulse_1.set_timer_length_high((uint8_t)(new_value & 0x07));
        pulse_1.set_length_counter(new_value >> 3);
    } else if (&value == pulse_2_envelope && write) {
        pulse_2.set_duty_cycle(new_value >> 6);
        pulse_2.set_length_counter_halt((bool) ((new_value >> 5) & 0x01));
        pulse_2.set_constant_volume_flag((bool) ((new_value >> 4) & 0x01));
        pulse_2.set_volume_divider_period((uint8_t(new_value & 0x0f)));
    } else if (&value == pulse_2_sweep && write) {
        pulse_2.set_sweep_enabled((bool) (new_value >> 7));
        pulse_2.set_sweep_divider_period((uint8_t)((new_value >> 4) & 0x07));
        pulse_2.set_sweep_negate_flag((bool) ((new_value >> 3) & 0x01));
        pulse_2.set_sweep_shift_count((uint8_t)(new_value & 0x07));
    } else if (&value == pulse_2_timer_low && write) {
        pulse_2.set_timer_length_low(new_value);
    } else if (&value == pulse_2_timer_high && write) {
        pulse_2.set_timer_length_high((uint8_t)(new_value & 0x07));
        pulse_2.set_length_counter(new_value >> 3);
    } else if (&value == triangle_linear_counter && write) {
        triangle.set_length_counter_halt(new_value >> 7);
        triangle.set_linear_counter_reload((uint8_t)(new_value & 0x7f));
    } else if (&value == triangle_timer_low && write) {
        triangle.set_timer_length_low(new_value);
    } else if (&value == triangle_timer_high && write) {
        triangle.set_timer_length_high((uint8_t)(new_value & 0x07));
        triangle.set_length_counter(new_value >> 3);
    } else if (&value == noise_envelope && write) {
        noise.set_length_counter_halt((bool) ((new_value >> 5) & 0x01));
        noise.set_constant_volume_flag((bool) ((new_value >> 4) & 0x01));
        noise.set_volume_divider_period((uint8_t(new_value & 0x0f)));
    } else if (&value == noise_mode && write) {
        noise.set_mode((bool) (new_value >> 7));
        noise.set_period((uint8_t)(new_value & 0x0f));
    } else if (&value == noise_length_counter && write) {
        noise.set_length_counter(new_value >> 3);
    } else if (&value == apu_status && write) {
        // TODO: the rest
        noise.set_length_counter_enabled((bool) ((new_value >> 3) & 0x01));
        triangle.set_length_counter_enabled((bool) ((new_value >> 2) & 0x01));
        pulse_2.set_length_counter_enabled((bool) ((new_value >> 1) & 0x01));
        pulse_1.set_length_counter_enabled((bool) (new_value & 0x01));
    } else if (&value == apu_status && !write) {
        // TODO: the rest
        value = (uint8_t)((frame_interrupt << 6) |
                          ((noise.get_length_counter() > 0) << 3) |
                          ((triangle.get_length_counter() > 0) << 2) |
                          ((pulse_2.get_length_counter() > 0) << 1) |
                          (pulse_1.get_length_counter() > 0));
        frame_interrupt = false;
    } else if (&value == apu_frame_counter && write) {
        frame_counter_mode = new_value >> 7;
        if (frame_counter_mode) {
            half_frame_tick();
            quarter_frame_tick();
        }

        irq_inhibit = (bool) (new_value >> 6 & 0x01);
        if (irq_inhibit)
            frame_interrupt = false;
        frame_counter = 0;
    }
}

void apu::half_frame_tick() {
    triangle.half_frame_tick();
    pulse_1.half_frame_tick();
    pulse_2.half_frame_tick();
    noise.half_frame_tick();
}

void apu::quarter_frame_tick() {
    triangle.quarter_frame_tick();
    pulse_1.quarter_frame_tick();
    pulse_2.quarter_frame_tick();
    noise.quarter_frame_tick();
}

void apu::set_interrupt_handler(::interrupt_handler *interrupt_handler) {
    this->interrupt_handler = interrupt_handler;
}
