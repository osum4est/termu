#include "mapper.h"

mapper::mapper(nes_rom *rom) {
    this->rom = rom;
    trainer = rom->get_trainer();
    prg_rom = rom->get_prg_rom();
    prg_ram = rom->get_prg_ram();
    chr_rom = rom->get_chr_rom();
    misc_rom = rom->get_misc_rom();
}

mapper::~mapper() = default;

nes_rom *mapper::get_rom() {
    return rom;
}
