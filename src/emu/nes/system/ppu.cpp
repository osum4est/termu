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

    rendering_enabled = false;
    in_vblank = false;

    sprite_eval_n = 0;
    sprite_eval_m = 0;
    sprite_eval_stage = 0;
    sprite_eval_sprites = 0;

    fine_x = 0;
    w = false;

    start_time = std::chrono::high_resolution_clock::now();
    benchmark_time = std::chrono::high_resolution_clock::now();
    current_cycle = 0;
    frame_cycle = 0;
    frames = 0;
}

void ppu::set_interrupt_handler(::interrupt_handler *interrupt_handler) {
    this->interrupt_handler = interrupt_handler;
}

void ppu::cycle() {

    if (benchmark_cycles > 5370000 && std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - benchmark_time).count() >= 1) {
        log->info(utils::string_format("PPU Clock rate: %.2fMHz / 5.37MHz", benchmark_cycles / 1e+6));
        benchmark_cycles = 0;
        benchmark_time = std::chrono::high_resolution_clock::now();
    }

    frame();

    benchmark_cycles++;
    current_cycle++;
}

void ppu::frame() {
    scanline(frame_cycle / 341, frame_cycle % 341);

    if (frame_cycle < 262 * 341 - 1)
        frame_cycle++;
    else
        frame_cycle = 0;
}

void ppu::scanline(int scanline, int tick) {
    // Visible scanlines
    if (scanline < 240 && rendering_enabled) {
        if (tick == 0)
            return;

        shift_regs(tick);
        prepare_bg(tick);
        prepare_sprites(tick);

        // TODO: prep for next scanline
        if (tick < 257)
            render_pixel(tick);
    }

    // Vertical blanking / post render scanlines
    if (scanline > 239 && scanline < 261) {
        if (scanline == 241 && tick == 1) {
            status_vblank = 1;
            in_vblank = true;
            if (ctrl_gen_nmi)
                interrupt_handler->set_nmi();
        }
        return;
    }

    // TODO: Prerender line, skip last cycle if odd
    if (scanline == 261) {
        if (tick == 1) {
            status_vblank = 0;
            in_vblank = false;
            status_sprite_0_hit = 0;
            status_sprite_overflow = 0;
        }

        if (tick > 279 && tick < 305 && rendering_enabled) {
            v_nametable = (uint8_t) ((v_nametable & 0x01) | (t_nametable & 0x02));
            v_fine_y = t_fine_y;
            v_coarse_y = t_coarse_y;
        }

        if (tick == 340) {
            frames++;

            if (rendering_enabled)
                display->render();

        }
    }
}

void ppu::shift_regs(int tick) {
    if ((tick > 1 && tick < 258) || (tick > 321 && tick < 338)) {
        shift_shift_reg(bg_shift_bitmap);
        shift_shift_reg(bg_shift_palette, bg_latch_palette);

        if (tick > 1 && tick < 258) {
            for (int i = 0; i < 8; i++) {
                if (sprite_counters[i] > 0)
                    sprite_counters[i]--;
                else if (sprite_counters[i] == 0)
                    shift_shift_reg(sprite_shift_bitmap[i]);
            }
        }
    }
}

void ppu::prepare_bg(int tick) {
    if (tick < 257) {
        int step = tick % 8;
        switch (step) {
            case 0: {
                hi_tile_byte = mem->get_ppu(
                        (uint16_t) ((tile_index_byte << 4) | v_fine_y | (ctrl_bg_table << 12) | 0x08));

                inc_v_x();
                if (tick == 256)
                    inc_v_y();
                break;
            }
            case 1: {
                bg_shift_bitmap[0] |= lo_tile_byte << 8;
                bg_shift_bitmap[1] |= hi_tile_byte << 8;

                uint8_t shift = (uint8_t) (((v_coarse_y & 0x02) << 1) | (v_coarse_x & 0x02));
                uint8_t palette = (uint8_t) ((attribute_byte >> shift) & 0x03);
                bg_latch_palette[0] = palette & (uint8_t) 0x01;
                bg_latch_palette[1] = (palette >> 1) & (uint8_t) 0x01;
            }
            case 2:
                tile_index_byte = mem->get_ppu(
                        (uint16_t) (0x2000 | (v_nametable << 10) | (v_coarse_y << 5) | v_coarse_x));
                break;
            case 4:
                attribute_byte = mem->get_ppu(
                        (uint16_t) (0x23c0 | (v_nametable << 10) | ((v_coarse_y >> 2) << 3) | (v_coarse_x >> 2)));
                break;
            case 6:
                lo_tile_byte = mem->get_ppu(
                        (uint16_t) ((tile_index_byte << 4) | v_fine_y | (ctrl_bg_table << 12)));
                break;
            default:
                break;
        }
    }

    if (tick == 257) {
        v_nametable = (uint8_t) ((v_nametable & 0x02) | (t_nametable & 0x01));
        v_coarse_x = t_coarse_x;
    }
}

void ppu::prepare_sprites(int tick) {
    bool even = tick % 2 == 0;

    // Secondary OAM clear
    if (tick < 65) {
        if (even) {
            mem->set_secondary_oam((uint16_t) (tick / 2 - 1), 0xff);
        }
    }

    // Sprite evaluation
    if (tick > 64 && tick < 257) {
        if (even && sprite_eval_n < 64) {
            if (sprite_eval_sprites < 8) {
                if (frames == 468)
                    frames = 468;
                // Load sprites into secondary oam while there's space and sprites left
                mem->set_secondary_oam((uint16_t) (sprite_eval_sprites * 4 + sprite_eval_stage),
                                       mem->get_oam((uint16_t) (sprite_eval_n * 4 + sprite_eval_stage)));

                if (sprite_eval_stage == 0) {
                    uint8_t y = mem->get_secondary_oam((uint16_t) (sprite_eval_sprites * 4));
                    if (is_y_in_range(y))
                        sprite_eval_stage++;
                    else
                        sprite_eval_n++;
                } else if (sprite_eval_stage == 3) {
                    sprite_eval_n++;
                    sprite_eval_sprites++;
                    sprite_eval_stage = 0;
                } else {
                    sprite_eval_stage++;
                }
            } else {
                // Calculate sprite overflow
                uint8_t y = mem->get_oam((uint16_t) (sprite_eval_n * 4 + sprite_eval_m));
                if (sprite_eval_stage == 0) {
                    if (is_y_in_range(y)) {
                        status_sprite_overflow = 1;
                        sprite_eval_stage++;
                    } else {
                        sprite_eval_n++;
                        sprite_eval_m = (uint8_t) ((sprite_eval_m + 1) & 0x03); // Bug here. On purpose!
                    }
                } else {
                    if (sprite_eval_m == 3)
                        sprite_eval_n++;
                    sprite_eval_m = (uint8_t) ((sprite_eval_m + 1) & 0x03);

                    if (sprite_eval_stage == 3)
                        sprite_eval_stage = 0;
                    else
                        sprite_eval_stage++;
                }
            }
        }
    }

    // Reset sprite eval variables for next scanline
    if (tick == 257) {
        sprite_eval_n = 0;
        sprite_eval_m = 0;
        sprite_eval_sprites = 0;
        sprite_eval_stage = 0;
    }

    // Sprite fetch
    if (tick > 256 && tick < 321) {
        int step = tick % 8;
        int sprite = (tick - 257) / 8;
        int scanline = frame_cycle / 341;
        switch (step) {
            case 0:
                // TODO: Support 8x16 sprites
                if (is_y_in_range(sprite_y_byte))
                    hi_tile_byte = mem->get_ppu((uint16_t) ((tile_index_byte << 4) | (scanline - sprite_y_byte) |
                                                            (ctrl_sprite_table << 12) | 0x08));
                else
                    hi_tile_byte = 0;

                break;
            case 1:
                sprite_y_byte = mem->get_secondary_oam((uint16_t) (sprite * 4));
                sprite_shift_bitmap[sprite - 1][0] = lo_tile_byte;
                sprite_shift_bitmap[sprite - 1][1] = hi_tile_byte;
                break;
            case 2:
                tile_index_byte = mem->get_secondary_oam((uint16_t) (sprite * 4 + 1));
                break;
            case 3:
                sprite_latch_attribute[sprite] = mem->get_secondary_oam((uint16_t) (sprite * 4 + 2));
                break;
            case 4:
                sprite_counters[sprite] = mem->get_secondary_oam((uint16_t) (sprite * 4 + 3));
                break;
            case 6:
                if (is_y_in_range(sprite_y_byte))
                    lo_tile_byte = mem->get_ppu((uint16_t) ((tile_index_byte << 4) | (scanline - sprite_y_byte) |
                                                            (ctrl_sprite_table << 12)));
                else
                    lo_tile_byte = 0;
                break;
            default:
                break;
        }
    }
}

bool ppu::is_y_in_range(int y) {
    int scanline = frame_cycle / 341;
    // TODO: Support 8x16 sprites
    return scanline - y >= 0 && scanline - y < 8;
}

void ppu::render_pixel(int tick) {
    // TODO: hide left clipping windows

    // Get background pixel
    uint8_t bitmap = get_shift_reg(bg_shift_bitmap);
    uint8_t palette = get_shift_reg(bg_shift_palette);
    uint16_t palette_offset = 0x3f00;

    // Get sprite pixel
    for (int i = 0; i < 8; i++) {
        if (sprite_counters[i] == 0) {
            uint8_t sprite_bitmap = get_shift_reg(sprite_shift_bitmap[i]);
            uint8_t sprite_attribute = sprite_latch_attribute[i];
            if (sprite_bitmap != 0) {
                // Sprite 0 hit
                int x = tick - 2;
                if (!status_sprite_0_hit && mask_bg_enable && mask_sprite_enable && x != 255 &&
                    is_y_in_range(mem->get_oam(0) + 1) && bitmap != 0 &&
                    !((!mask_bg_left_col || !mask_sprite_left_col) && (x >= 0 && x < 8))) {
                    status_sprite_0_hit = 1;
                }

                uint8_t priority = (sprite_attribute >> 5) & (uint8_t) 0x01;
                if (bitmap == 0 || !priority) {
                    bitmap = sprite_bitmap;
                    palette = sprite_attribute & (uint8_t) 0x03;
                    palette_offset = 0x3f10;
                }
                break;
            }
        }
    }

    // Output color
    uint8_t color = bitmap ? mem->get_ppu((uint16_t) (palette_offset + palette * 4 + bitmap)) : mem->get_ppu(0x3f00);
    display->set_pixel(v_coarse_x * 8 + (tick % 8) - 1, v_coarse_y * 8 + v_fine_y, color);
}

uint8_t ppu::get_shift_reg(uint16_t *shift_reg) {
    return (uint8_t) ((((shift_reg[1] >> (15 - fine_x)) & 0x01) << 1) | ((shift_reg[0] >> (15 - fine_x)) & 0x01));
}

uint8_t ppu::get_shift_reg(uint8_t *shift_reg) {
    return (uint8_t) ((((shift_reg[1] >> (7 - fine_x)) & 0x01) << 1) | ((shift_reg[0] >> (7 - fine_x)) & 0x01));
}

void ppu::shift_shift_reg(uint16_t *shift_reg, uint8_t *latch) {
    shift_reg[0] <<= 1;
    if (latch != nullptr)
        shift_reg[0] |= latch[0] & 0x01;

    shift_reg[1] <<= 1;
    if (latch != nullptr)
        shift_reg[1] |= latch[1] & 0x01;
}

void ppu::shift_shift_reg(uint8_t *shift_reg, uint8_t *latch) {
    shift_reg[0] <<= 1;
    if (latch != nullptr)
        shift_reg[0] |= latch[0] & 0x01;

    shift_reg[1] <<= 1;
    if (latch != nullptr)
        shift_reg[1] |= latch[1] & 0x01;
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
    } else if (&value == oam_data && write && (in_vblank || !rendering_enabled)) {
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

        if (in_vblank || !rendering_enabled) {
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

    rendering_enabled = mask_bg_enable || mask_sprite_enable;
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
