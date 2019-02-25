#include "nes_emulator.h"
#include "../core/exceptions/emu_exception.h"

nes_emulator::nes_emulator() {
    rom = nullptr;
    nes = new ::nes();
}

nes_emulator::~nes_emulator() {
    delete rom;
    delete nes;
}

bool nes_emulator::extension_matches(const std::string &extension) {
    return extension == "nes";
}

rom *nes_emulator::load_rom(const rom_path &rom_path) {
    if (rom != nullptr)
        throw emu_exception("A rom is already loaded.");

    nes->stop();
    rom = new nes_rom(rom_path);
    nes->insert_cartridge(rom);
    return rom;
}

rom *nes_emulator::get_loaded_rom() {
    return rom;
}

void nes_emulator::start() {
    nes->start();
}
