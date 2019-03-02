//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_PPU_H
#define TERMU_PPU_H

#include <chrono>
#include <spdlog/spdlog.h>
#include "../../../utils/utils.h"
#include "mem.h"
#include "interrupt_handler.h"
#include "../../core/display/emu_display.h"

class ppu {
    std::unique_ptr<spdlog::logger> log = utils::get_logger("ppu");

    ::mem *mem;
	emu_display *display;
    ::interrupt_handler *interrupt_handler;

    uint64_t current_cycle;
    uint64_t benchmark_cycles;
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point benchmark_time;

    uint8_t *ppu_ctrl;
    uint8_t *ppu_mask;
    uint8_t *ppu_status;
    uint8_t *oam_addr;
    uint8_t *oam_data;
    uint8_t *ppu_scroll;
    uint8_t *ppu_addr;
    uint8_t *ppu_data;

    uint16_t v;
    uint16_t t;
    uint8_t x;
    bool w;

public:
    explicit ppu(::mem *mem, emu_display* display);

    void start();

    void set_interrupt_handler(::interrupt_handler *interrupt_handler);

    /**
     * Cycles the PPU.
     */
    void cycle();

private:
    void frame();

    void scanline(int line_num);

    void inc_v();

    void inc_v_x();

    void inc_v_y();

    void reg_handler(uint8_t &value, uint8_t new_value, bool write);

	void setup_palette();
};

#endif //TERMU_PPU_H
