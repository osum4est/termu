#include <cstdio>
#include <sstream>
#include "mapper001_mmc1.h"
#include "../../../../utils/utils.h"
#include "../../../core/exceptions/emu_exception.h"

mapper001_mmc1::mapper001_mmc1(nes_rom *rom) : mapper(rom) {
    if (rom->get_chr_rom_size() == 0)
        chr = chr_ram;
    else
        chr = chr_rom;

    prg_bank = 0;
    chr_bank_0 = 0;
    chr_bank_1 = 0;
    shift = 0;
    shift_amount = 0;
    last_write_cycle = 0;

    mirroring = single_a;
    prg_bank_mode = switch_first_16k;
    chr_bank_mode = switch_4k;
}

uint8_t &mapper001_mmc1::map_prg(uint16_t addr, bool write) {
    if (addr < 0x6000)
        throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));

    if (addr < 0x8000)
        return prg_ram[(addr - 0x6000) % rom->get_prg_ram_size()];

    if (prg_bank_mode == switch_32k)
        return prg_rom[prg_bank * 0x4000 + (addr - 0x8000)];

    if (addr < 0xc000 && prg_bank_mode == switch_last_16k)
        return prg_rom[addr - 0x8000];
    else if (addr < 0xc000 && prg_bank_mode == switch_first_16k)
        return prg_rom[prg_bank * 0x4000 + (addr - 0x8000)];

    if (prg_bank_mode == switch_last_16k)
        return prg_rom[prg_bank * 0x4000 + (addr - 0xc000)];
    else if (prg_bank_mode == switch_first_16k)
        return prg_rom[(rom->get_prg_rom_size() - 0x4000) + (addr - 0xc000)];

    throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));
}

uint8_t &mapper001_mmc1::map_chr(uint16_t addr, bool write) {
    if (chr_bank_mode == switch_8k)
        return chr[chr_bank_0 * 0x1000 + addr];

    if (addr < 0x1000)
        return chr[chr_bank_0 * 0x1000 + addr];

    if (addr < 0x2000)
        return chr[chr_bank_1 * 0x1000 + (addr - 0x1000)];

    throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));
}

void mapper001_mmc1::write_control(uint8_t b) {
    switch (b & 0x03) {
        case 0:
            mirroring = mirroring_type::single_a;
            break;
        case 1:
            mirroring = mirroring_type::single_b;
            break;
        case 2:
            mirroring = mirroring_type::vertical;
            break;
        case 3:
            mirroring = mirroring_type::horizontal;
            break;
    }

    switch ((b >> 2) & 0x03) {
        case 0:
        case 1:
            prg_bank_mode = prg_bank_mode::switch_32k;
            break;
        case 2:
            prg_bank_mode = prg_bank_mode::switch_last_16k;
            break;
        case 3:
            prg_bank_mode = prg_bank_mode::switch_first_16k;
            break;
    }

    switch ((b >> 4) & 0x01) {
        case 0:
            chr_bank_mode = chr_bank_mode::switch_8k;
            break;
        case 1:
            chr_bank_mode = chr_bank_mode::switch_4k;
            break;
    }
}

void mapper001_mmc1::write_chr_bank_0(uint8_t b) {
    if (chr_bank_mode == switch_8k)
        chr_bank_0 = (uint8_t) ((b & 0x1e) * 2);
    else
        chr_bank_0 = (uint8_t) (b & 0x1f);
}

void mapper001_mmc1::write_chr_bank_1(uint8_t b) {
    if (chr_bank_mode != switch_8k)
        chr_bank_1 = (uint8_t) (b & 0x1f);
}

void mapper001_mmc1::write_prg_bank(uint8_t b) {
    if (prg_bank_mode == switch_32k)
        prg_bank = (uint8_t) ((b & 0x0e) * 2);
    else
        prg_bank = (uint8_t) (b & 0x0f);
}

void mapper001_mmc1::set_prg(uint16_t addr, uint8_t b) {
    if (addr < 0x8000) {
        mapper::set_prg(addr, b);
        return;
    }

    if (last_write_cycle - *cpu_cycles < 2)
        return;

    last_write_cycle = *cpu_cycles;

    if (b & 0x80) {
        shift = 0;
        shift_amount = 0;
        prg_bank_mode = switch_first_16k;
    } else {
        shift >>= 1;
        shift |= (b & 0x01) << 7;
        shift_amount++;

        if (shift_amount == 5) {
            shift >>= 3;
            switch ((addr >> 13) & 0x03) {
                case 0:
                    write_control(shift);
                    break;
                case 1:
                    write_chr_bank_0(shift);
                    break;
                case 2:
                    write_chr_bank_1(shift);
                    break;
                case 3:
                    write_prg_bank(shift);
                    break;
                default:
                    break;
            }
            shift = 0;
            shift_amount = 0;
        }
    }
}