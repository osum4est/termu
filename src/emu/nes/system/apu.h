//
// Created by Forrest Jones on 2019-03-11.
//

#ifndef TERMU_APU_H
#define TERMU_APU_H


#include <RtAudio.h>
#include "mem.h"

class apu {
    ::mem *mem;

    RtAudio dac;
    RtAudio::StreamParameters stream_params;
    unsigned int buffer_size;

    int last_frame = 0;

    static int gen_audio(void *output_buffer, void *input_buffer, unsigned int frames,
                         double time, RtAudioStreamStatus status, void *data);

public:
    explicit apu(::mem *mem);

    void start();

    void stop();
};


#endif //TERMU_APU_H
