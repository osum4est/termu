#include "mapper.h"

mapper::mapper(nes_rom *rom) {
    this->rom = rom;
    mirroring = rom->get_mirroring_type();
    trainer = rom->get_trainer();
    prg_rom = rom->get_prg_rom();
    prg_ram = rom->get_prg_ram();
    chr_rom = rom->get_chr_rom();
    misc_rom = rom->get_misc_rom();

    if (rom->get_chr_ram_size() == 0)
        chr_ram = new uint8_t[32 * 1024];
    else
        chr_ram = new uint8_t[rom->get_chr_ram_size()];
}

mapper::~mapper() {
    delete[] chr_ram;
}

uint8_t &mapper::get_prg(uint16_t addr) {
    return map_prg(addr, false);
}

void mapper::set_prg(uint16_t addr, uint8_t b) {
    map_prg(addr, true) = b;
}

uint8_t &mapper::get_chr(uint16_t addr) {
    return map_chr(addr, false);
}

void mapper::set_chr(uint16_t addr, uint8_t b) {
    map_chr(addr, true) = b;
}

mirroring_type mapper::get_mirroring_type() {
    return mirroring;
}

nes_rom *mapper::get_rom() {
    return rom;
}

void mapper::set_cpu_cycles(uint64_t *cycles) {
    this->cpu_cycles = cycles;
}
