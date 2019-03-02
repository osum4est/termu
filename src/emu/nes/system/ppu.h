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

    // PPU CTRL
    uint8_t ctrl_base_nametable;
    uint8_t ctrl_vram_increment;
    uint8_t ctrl_sprite_table;
    uint8_t ctrl_bg_table;
    uint8_t ctrl_sprite_size;
    uint8_t ctrl_master_slave;
    uint8_t ctrl_gen_nmi;

    // PPU MASK
    uint8_t mask_grayscale;
    uint8_t mask_bg_left_col;
    uint8_t mask_sprite_left_col;
    uint8_t mask_bg_enable;
    uint8_t mask_sprite_enable;
    uint8_t mask_emphasize_red;
    uint8_t mask_emphasize_green;
    uint8_t mask_emphasize_blue;

    // PPU STATUS
    uint8_t status_sprite_overflow;
    uint8_t status_sprite_0_hit;
    uint8_t status_vblank;

    // V
    uint8_t v_coarse_x;
    uint8_t v_coarse_y;
    uint8_t v_nametable;
    uint8_t v_fine_y;

    // T
    uint8_t t_coarse_x;
    uint8_t t_coarse_y;
    uint8_t t_nametable;
    uint8_t t_fine_y;

    // State
    bool rendering;

    uint8_t fine_x;
    bool w;

public:
    explicit ppu(::mem *mem, emu_display *display);

    void start();

    void set_interrupt_handler(::interrupt_handler *interrupt_handler);

    /**
     * Cycles the PPU.
     */
    void cycle();

private:
    void frame();

    void scanline(int line_num);

    void inc_v_x();

    void inc_v_y();

    void reg_handler(uint8_t &value, uint8_t new_value, bool write);

    void set_ctrl(uint8_t ctrl);

    void set_mask(uint8_t mask);

    uint8_t get_status();

    void set_v(uint16_t v);

    uint16_t get_v();

    void setup_palette();
};

#endif //TERMU_PPU_H
