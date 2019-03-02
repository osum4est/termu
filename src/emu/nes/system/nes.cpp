#include "nes.h"

nes::~nes() {
    delete cpu;
    delete ppu;
    delete mem;
}

void nes::insert_cartridge(nes_rom *rom) {
    this->cartridge = rom;
}

void nes::set_display(emu_display* display) {
	this->display = display;
}


void nes::start() {
	if (cartridge == nullptr) {
		throw emu_exception("A rom must be loaded before starting the emulator.");
	}

	if (display == nullptr) {
		throw emu_exception("A display must be set before starting the emulator.");
	}

    mem = new ::mem(cartridge);
    ppu = new ::ppu(mem, display);
    cpu = new ::cpu(mem, ppu);

    cpu->start();
}

void nes::stop() {
}
