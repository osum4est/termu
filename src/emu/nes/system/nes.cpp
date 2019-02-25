#include "nes.h"

nes::~nes() {
    delete cpu;
    delete ppu;
    delete mem;
}

void nes::insert_cartridge(nes_rom *rom) {
    this->cartridge = rom;
}

void nes::start() {
    mem = new ::mem(cartridge);
    ppu = new ::ppu(mem);
    cpu = new ::cpu(ppu, mem);

    cpu->start();
}

void nes::stop() {
}
