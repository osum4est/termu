#include "ppu.h"

ppu::ppu(::mem *mem) {
    this->mem = mem;
}


void ppu::start() {
    // TODO: other init stuff

    start_time = std::chrono::high_resolution_clock::now();
    benchmark_time = std::chrono::high_resolution_clock::now();
    current_cycle = 0;
}

void ppu::cycle() {
    benchmark_cycles++;
    current_cycle++;

    if (benchmark_cycles > 5370000 && std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - benchmark_time).count() >= 1) {
        log->info(utils::string_format("PPU Clock rate: %.2fMHz / 5.37MHz", benchmark_cycles / 1e+6));
        benchmark_cycles = 0;
        benchmark_time = std::chrono::high_resolution_clock::now();
    }
}
