//
// Created by Forrest Jones on 2019-03-11.
//

#include <thread>
#include <cmath>
#include "apu.h"

apu::apu(::mem *mem) {
    this->mem = mem;
}

void apu::start() {
    if (dac.getDeviceCount() < 1)
        throw emu_exception("Could not find any audio devices.");

    buffer_size = 2048;
    stream_params.deviceId = dac.getDefaultOutputDevice();
    stream_params.firstChannel = 0;
    stream_params.nChannels = 2;


    dac.openStream(&stream_params, nullptr, RTAUDIO_FLOAT64, 48000, &buffer_size, &gen_audio, this);
    dac.startStream();
}

void apu::stop() {
    dac.stopStream();
}

int apu::gen_audio(void *output_buffer, void *input_buffer, unsigned int frames,
                   double time, RtAudioStreamStatus status, void *data) {
    apu *self = static_cast<apu *>(data);

    if (status)
        throw emu_exception("Audio stream underflow.");

    double *buffer = static_cast<double *>(output_buffer);
    for (int i = 0; i < frames; i++) {
        int f = (i + self->last_frame) % 48000;
        *buffer++ = std::sin(440 * 2 * 3.14159265358979323846 * (double) f / 48000) / 10;
        *buffer++ = std::sin(440 * 2 * 3.14159265358979323846 * (double) f / 48000) / 10;
    }
    self->last_frame += frames;
    self->last_frame %= 48000;

    return 0;
}
