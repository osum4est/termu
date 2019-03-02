#include <cstdio>
#include <sstream>
#include "mapper0000.h"
#include "../../../../utils/utils.h"
#include "../../../core/exceptions/emu_exception.h"

mapper0000::mapper0000(nes_rom *rom) : mapper(rom) {
}

uint8_t &mapper0000::get_prg(uint16_t addr) {
    if (addr < 0x6000)
        throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));

    if (addr < 0x8000)
        return prg_ram[(addr - 0x6000) % rom->get_prg_ram_size()];

    return prg_rom[(addr - 0x8000) % rom->get_prg_rom_size()];
}

void mapper0000::set_prg(uint16_t addr, uint8_t b) {
    if (addr < 0x6000)
        throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));

    if (addr < 0x8000)
        prg_ram[(addr - 0x6000) % rom->get_prg_ram_size()] = b;

    else
        prg_rom[(addr - 0x8000) % rom->get_prg_rom_size()] = b;
}

uint8_t &mapper0000::get_chr(uint16_t addr) {
    if (addr < 0x2000)
        return chr_rom[addr];

    throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));
}

void mapper0000::set_chr(uint16_t addr, uint8_t b) {
    if (addr < 0x2000)
        chr_rom[addr] = b;

    else
        throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));
}
