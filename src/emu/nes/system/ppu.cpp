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

    v = 0;
    t = 0;
    x = 0;
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
    v = 0;
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
        int coarse_x = v & 0x001f;
        int coarse_y = (v & 0x03e0) >> 5;
        int fine_y = v >> 12;

        int attr_x = (coarse_x % 4) / 2;
        int attr_y = (coarse_y % 4) / 2;

        uint8_t pattern = mem->get_ppu((uint16_t) (0x2000 | (v & 0x0fff)));
        uint8_t attribute = mem->get_ppu((uint16_t) (0x23c0 | (v & 0x0c00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07)));

        uint16_t pattern_addr = (uint16_t) ((pattern << 4) | fine_y | ((*ppu_ctrl & 0x10) << 8));
        uint8_t pattern_lo = mem->get_ppu(pattern_addr);
        uint8_t pattern_hi = mem->get_ppu((uint16_t) (pattern_addr | 0x08));

        // TODO: Put pixels in shift regs

        uint8_t palette = (uint8_t) ((attribute >> (attr_y * 4 + attr_x * 2)) & 0x03);

        for (int fine_x = 0; fine_x < 8; fine_x++) {
            uint8_t pixel = (uint8_t) (((pattern_hi >> (7 - fine_x) & 0x01) << 1) | ((pattern_lo >> (7 - fine_x)) & 0x01));

            uint8_t color;
            if (pixel == 0) {
                color = mem->get_ppu(0x3f00);
            } else {
                color = mem->get_ppu((uint16_t) (0x3f00 + palette * 4 + pixel));
            }

            display->set_pixel(coarse_x * 8 + fine_x, coarse_y * 8 + fine_y, color);
        }

        inc_v_x();
//        uint8_t attribute_table = //
//        uint16_t tile_bitmap = ...;
    }


}

void ppu::inc_v() {
    if ((*ppu_ctrl & 0x04) == 0x04)
        inc_v_y();
    else
        inc_v_x();
}

void ppu::inc_v_x() {
    if ((v & 0x001f) == 31) {
        v &= 0xffe0;
        v ^= 0x0400;
    } else {
        v++;
    }
}

void ppu::inc_v_y() {
    if ((v & 0x7000) != 0x7000) {
        v += 0x1000;
    } else {
        v &= ~0x7000;
        int y = (v & 0x03e0) >> 5;
        if (y == 29) {
            y = 0;
            v ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y += 1;
        }
        v = (uint16_t) ((v & ~0x03e0) | (y << 5));
    }
}

void ppu::reg_handler(uint8_t &value, uint8_t new_value, bool write) {
    // TODO: the rest of them
    if (&value == ppu_ctrl && write) {
        t = (uint8_t) ((t & 0x73ff) | ((new_value & 0x03) << 10));
    } else if (&value == ppu_status && !write) {
        w = false;
    } else if (&value == oam_data && write) { // TODO: && !rendering
        // TODO: oam_data read
        mem->set_oam(*oam_addr, new_value);
        (*oam_addr)++;
    } else if (&value == ppu_addr && write) {
        if (!w) {
            t = (uint16_t) (((new_value & 0x3f) << 8) | (t & 0xff)); // Upper byte
        } else {
            t = (uint16_t) (new_value | (t & 0xff00)); // Lower byte
            v = t;
        }
        w = !w;
    } else if (&value == ppu_data && write) { // TODO: acts weird during rendering
        mem->set_ppu(v, new_value);
        if ((*ppu_ctrl & 0x04) == 0x04) {
            v += 32;
        } else {
            v++;
        }
    } else if (&value == ppu_data && !write) {
        value = mem->get_ppu(v);
        if ((*ppu_ctrl & 0x04) == 0x04) {
            v += 32;
        } else {
            v++;
        }
    } else if (&value == ppu_status && !write) {
        // TODO: Actually return the status
        value = 0x80;
    }
}

void ppu::setup_palette() {
    int palette[64];
    nes_2C02_palette().get_palette(palette);

    for (int i = 0; i < 64; i++)
        display->set_palette(palette, 64);
}
