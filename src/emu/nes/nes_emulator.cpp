#include "nes_emulator.h"
#include "../core/exceptions/emu_exception.h"
#include "input/nes_controller.h"

nes_emulator::~nes_emulator() {
    stop();
}

bool nes_emulator::extension_matches(const std::string &extension) {
    return extension == "nes";
}

rom *nes_emulator::load_rom(const rom_path &rom_path) {
    if (cartridge != nullptr)
        throw emu_exception("A rom is already loaded.");

    cartridge = new nes_rom(rom_path);
    return cartridge;
}

rom *nes_emulator::get_loaded_rom() {
    return cartridge;
}

void nes_emulator::set_display(emu_display* display) {
    this->display = display;
}

void nes_emulator::set_controller(int index, emu_controller *controller) {
    if (index > 1) {
        throw emu_exception(utils::string_format("Cannot set controller index %d.", index));
    }

    if (inputs[index] != nullptr)
        delete inputs[index];

    inputs[index] = new nes_controller(controller);
}


void nes_emulator::start() {
    if (started) {
        throw emu_exception("This emulator has already been started.");
    }

    if (cartridge == nullptr) {
        throw emu_exception("A rom must be loaded before starting the emulator.");
    }

    if (display == nullptr) {
        throw emu_exception("A display must be set before starting the emulator.");
    }

    mem = new ::mem(cartridge, inputs);
    ppu = new ::ppu(mem, display);
    apu = new ::apu(mem);
    cpu = new ::cpu(mem, ppu);

    ppu->start();
    apu->start();
    cpu->start();
}

void nes_emulator::stop() {
    apu->stop();
    cpu->stop();

    delete cartridge;
    delete inputs[0];
    delete inputs[1];
    delete cpu;
    delete ppu;
    delete apu;
    delete mem;
}
