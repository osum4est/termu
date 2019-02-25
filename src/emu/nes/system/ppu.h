//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_PPU_H
#define TERMU_PPU_H

#include <chrono>
#include <spdlog/spdlog.h>
#include "../../../utils/utils.h"
#include "mem.h"

class ppu {
    std::unique_ptr<spdlog::logger> log = utils::get_logger("ppu");

    mem *mem;

    uint64_t current_cycle;

    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point benchmark_time;
    uint64_t benchmark_cycles;

public:
    explicit ppu(::mem *mem);

    void start();

    /**
     * Cycles the PPU.
     */
    void cycle();
};

#endif //TERMU_PPU_H
