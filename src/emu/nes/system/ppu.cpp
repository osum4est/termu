#include "ppu.h"
#include "../palettes/nes_2C02_palette.h"

ppu::ppu(::mem *mem, emu_display *display) {
    this->mem = mem;
    this->display = display;
}

void ppu::start() {
    display->init(256, 224);
    setup_palette();

    ppu_ctrl = &mem->get_cpu(0x2000);
    ppu_mask = &mem->get_cpu(0x2001);
    ppu_status = &mem->get_cpu(0x2002);
    oam_addr = &mem->get_cpu(0x2003);
    oam_data = &mem->get_cpu(0x2004);
    ppu_scroll = &mem->get_cpu(0x2005);
    ppu_addr = &mem->get_cpu(0x2006);
    ppu_data = &mem->get_cpu(0x2007);

    mem->set_ppu_reg_handler(
            std::bind(&ppu::reg_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    ctrl_base_nametable = 0;
    ctrl_vram_increment = 0;
    ctrl_sprite_table = 0;
    ctrl_bg_table = 0;
    ctrl_sprite_size = 0;
    ctrl_master_slave = 0;
    ctrl_gen_nmi = 0;

    mask_grayscale = 0;
    mask_bg_left_col = 0;
    mask_sprite_left_col = 0;
    mask_bg_enable = 0;
    mask_sprite_enable = 0;
    mask_emphasize_red = 0;
    mask_emphasize_green = 0;
    mask_emphasize_blue = 0;

    status_sprite_overflow = 0;
    status_sprite_0_hit = 0;
    status_vblank = 0;

    v_coarse_x = 0;
    v_coarse_y = 0;
    v_nametable = 0;
    v_fine_y = 0;

    t_coarse_x = 0;
    t_coarse_y = 0;
    t_nametable = 0;
    t_fine_y = 0;

    fine_x = 0;
    w = false;

    start_time = std::chrono::high_resolution_clock::now();
    benchmark_time = std::chrono::high_resolution_clock::now();
    current_cycle = 0;
}

void ppu::set_interrupt_handler(::interrupt_handler *interrupt_handler) {
    this->interrupt_handler = interrupt_handler;
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

    // For now, just render 1 full frame
    if (current_cycle == 128492) {
        interrupt_handler->set_nmi();
    }

    if (current_cycle == 248492) {
        interrupt_handler->set_nmi();
    }

    // if (current_cycle == 348492) {
    // interrupt_handler->set_nmi();
    // }

    if (current_cycle == 448492) {
        frame();
    }
}

void ppu::frame() {
    // TODO:  Prerender scanline


    v_coarse_x = t_coarse_x;
    v_coarse_y = t_coarse_y;
    v_nametable = t_nametable;
    v_fine_y = t_fine_y;
    for (int i = 0; i < 239; i++) {
        scanline(i);
        inc_v_y();
    }
    display->render();
}

void ppu::scanline(int line_num) {
    // TODO:  Idle cycle

    // TODO: Split into cycles
    for (int i = 0; i < 256 / 8; i++) {
        int attr_x = (v_coarse_x % 4) / 2;
        int attr_y = (v_coarse_y % 4) / 2;

        uint8_t pattern = mem->get_ppu((uint16_t) (0x2000 | (v_nametable << 10) | (v_coarse_y << 5) | v_coarse_x));
        uint8_t attribute =
                mem->get_ppu((uint16_t) (0x23c0 | (v_nametable << 10) | ((v_coarse_y >> 2) << 3) | (v_coarse_x >> 2)));

        uint16_t pattern_addr = (uint16_t) ((pattern << 4) | v_fine_y | ((*ppu_ctrl & 0x10) << 8));
        uint8_t pattern_lo = mem->get_ppu(pattern_addr);
        uint8_t pattern_hi = mem->get_ppu((uint16_t) (pattern_addr | 0x08));

        // TODO: Put pixels in shift regs

        uint8_t palette = (uint8_t) ((attribute >> (attr_y * 4 + attr_x * 2)) & 0x03);

        for (int j = 0; j < 8; j++) {
            uint8_t pixel = (uint8_t) (((pattern_hi >> (7 - fine_x) & 0x01) << 1) |
                                       ((pattern_lo >> (7 - fine_x)) & 0x01));

            uint8_t color;
            if (pixel == 0) {
                color = mem->get_ppu(0x3f00);
            } else {
                color = mem->get_ppu((uint16_t) (0x3f00 + palette * 4 + pixel));
            }

            display->set_pixel(v_coarse_x * 8 + fine_x, v_coarse_y * 8 + v_fine_y, color);

            fine_x++;
            fine_x &= 0x07;
        }

        inc_v_x();
//        uint8_t attribute_table = //
//        uint16_t tile_bitmap = ...;
    }
}

void ppu::inc_v_x() {
    if (v_coarse_x == 31) {
        v_coarse_x = 0;
        v_nametable ^= 0x1;
    } else {
        v_coarse_x++;
    }
}

void ppu::inc_v_y() {
    if (v_fine_y < 7) {
        v_fine_y++;
    } else {
        v_fine_y = 0;
        if (v_coarse_y == 29) {
            v_coarse_y = 0;
            v_nametable ^= 0x2;
        } else if (v_coarse_y == 31) {
            v_coarse_y = 0;
        } else {
            v_coarse_y++;
        }
    }
}

void ppu::reg_handler(uint8_t &value, uint8_t new_value, bool write) {
    if (&value == ppu_ctrl && write) {
        set_ctrl(new_value);
    } else if (&value == ppu_mask && write) {
        set_mask(new_value);
    } else if (&value == ppu_status && !write) {
        value = get_status();
    } else if (&value == oam_data && write && !rendering) {
        mem->set_oam(*oam_addr, new_value);
        (*oam_addr)++;
    } else if (&value == oam_addr && !write) {
        value = mem->get_oam(*oam_addr);
    } else if (&value == ppu_scroll && write) {
        if (!w) {
            t_coarse_x = new_value >> 3;
            fine_x = new_value & (uint8_t) 0x07;
        } else {
            t_coarse_y = new_value >> 3;
            t_fine_y = new_value & (uint8_t) 0x07;
        }
        w = !w;
    } else if (&value == ppu_addr && write) {
        if (!w) {
            t_coarse_y = (uint8_t) ((t_coarse_y & 0x07) | ((new_value & 0x03) << 3));
            t_nametable = (new_value >> 2) & (uint8_t) 0x03;
            t_fine_y = (new_value >> 4) & (uint8_t) 0x03;
        } else {
            t_coarse_x = new_value & (uint8_t) 0x1f;
            t_coarse_y = (uint8_t) ((t_coarse_y & 0x18) | (new_value >> 5));

            v_coarse_x = t_coarse_x;
            v_coarse_y = t_coarse_y;
            v_nametable = t_nametable;
            v_fine_y = t_fine_y;
        }
        w = !w;
    } else if (&value == ppu_data) {
        if (write)
            mem->set_ppu(get_v(), new_value);
        else
            value = mem->get_ppu(get_v());

        if (status_vblank || !rendering) {
            if (ctrl_vram_increment) {
                set_v((uint16_t) (get_v() + 32));
            } else {
                set_v((uint16_t) (get_v() + 1));
            }
        } else {
            inc_v_x();
            inc_v_y();
        }
    }
}

void ppu::set_ctrl(uint8_t ctrl) {
    ctrl_base_nametable = ctrl & (uint8_t) 0x03;
    ctrl_vram_increment = (ctrl >> 2) & (uint8_t) 0x01;
    ctrl_sprite_table = (ctrl >> 3) & (uint8_t) 0x01;
    ctrl_bg_table = (ctrl >> 4) & (uint8_t) 0x01;
    ctrl_sprite_size = (ctrl >> 5) & (uint8_t) 0x01;
    ctrl_master_slave = (ctrl >> 6) & (uint8_t) 0x01;
    ctrl_gen_nmi = (ctrl >> 7) & (uint8_t) 0x01;
    if (ctrl_gen_nmi && status_vblank)
        interrupt_handler->set_nmi();

    t_nametable = ctrl_base_nametable;
}

void ppu::set_mask(uint8_t mask) {
    mask_grayscale = mask & (uint8_t) 0x01;
    mask_bg_left_col = (mask >> 1) & (uint8_t) 0x01;
    mask_sprite_left_col = (mask >> 2) & (uint8_t) 0x01;
    mask_bg_enable = (mask >> 3) & (uint8_t) 0x01;
    mask_sprite_enable = (mask >> 4) & (uint8_t) 0x01;
    mask_emphasize_red = (mask >> 5) & (uint8_t) 0x01;
    mask_emphasize_green = (mask >> 6) & (uint8_t) 0x01;
    mask_emphasize_blue = (mask >> 7) & (uint8_t) 0x01;
}

uint8_t ppu::get_status() {
    uint8_t status = 0;
    status |= status_sprite_overflow << 5;
    status |= status_sprite_0_hit << 6;
    status |= status_vblank << 7;
    status_vblank = 0;
    w = false;

    return status;
}

void ppu::set_v(uint16_t v) {
    v_coarse_x = (uint8_t) (v & 0x1f);
    v_coarse_y = (uint8_t) ((v >> 5) & 0x1f);
    v_nametable = (uint8_t) ((v >> 10) & 0x03);
    v_fine_y = (uint8_t) ((v >> 12) & 0x07);
}

uint16_t ppu::get_v() {
    uint16_t v = 0;
    v |= v_coarse_x;
    v |= v_coarse_y << 5;
    v |= v_nametable << 10;
    v |= v_fine_y << 12;

    return v;
}

void ppu::setup_palette() {
    int palette[64];
    nes_2C02_palette().get_palette(palette);
    display->set_palette(palette, 64);
}
