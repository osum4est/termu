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

    uint32_t frame_cycle;
    uint64_t frames;

    uint8_t *oam;
    uint8_t *sec_oam;

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
    bool rendering_enabled;
    bool in_vblank;
    uint8_t tile_index_byte;
    uint8_t attribute_byte;
    uint8_t lo_tile_byte;
    uint8_t hi_tile_byte;
    uint8_t sprite_y_byte;

    // Shift registers
    // Lo byte is at 0
    uint16_t bg_shift_bitmap[2];
    uint8_t bg_shift_palette[2];
    uint8_t bg_latch_palette[2];

    uint8_t sprite_shift_bitmap[8][2];
    uint8_t sprite_latch_attribute[8];
    uint8_t sprite_counters[8];

    uint8_t sprite_eval_n;
    uint8_t sprite_eval_m;
    uint8_t sprite_eval_stage;
    uint8_t sprite_eval_sprites;

    uint8_t fine_x;
    bool w;

public:
    explicit ppu(::mem *mem, emu_display *display);

    ~ppu();

    void start();

    void set_interrupt_handler(::interrupt_handler *interrupt_handler);

    /**
     * Cycles the PPU.
     */
    void cycle();

private:
    void frame();

    void scanline(int scanline, int tick);

    void shift_regs(int tick);

    void prepare_bg(int tick);

    void prepare_sprites(int tick);

    bool is_y_in_range(int y);

    void render_pixel(int tick);

    uint8_t get_shift_reg(uint16_t *shift_reg);

    uint8_t get_shift_reg(uint8_t *shift_reg);

    void shift_shift_reg(uint16_t *shift_reg, uint8_t *latch = nullptr);

    void shift_shift_reg(uint8_t *shift_reg, uint8_t *latch = nullptr);

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
