#include <cstdio>
#include <sstream>
#include "mapper000_nrom.h"
#include "../../../../utils/utils.h"
#include "../../../core/exceptions/emu_exception.h"

mapper000_nrom::mapper000_nrom(nes_rom *rom) : mapper(rom) {
}

uint8_t &mapper000_nrom::map_prg(uint16_t addr, bool write) {
    if (addr < 0x6000)
        throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));

    if (addr < 0x8000)
        return prg_ram[(addr - 0x6000) % rom->get_prg_ram_size()];

    return prg_rom[(addr - 0x8000) % rom->get_prg_rom_size()];
}

uint8_t &mapper000_nrom::map_chr(uint16_t addr, bool write) {
    if (addr < 0x2000)
        return chr_rom[addr];

    throw emu_exception(utils::string_format("Cannot access memory at %x.", addr));
}
