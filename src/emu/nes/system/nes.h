//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_NES_H
#define TERMU_NES_H

#include "../rom/nes_rom.h"
#include "ppu.h"
#include "cpu.h"

class nes {
    nes_rom *cartridge;
    cpu *cpu;
    ppu *ppu;
    mem *mem;

public:
    ~nes();

    void insert_cartridge(nes_rom *rom);

    void start();

    void stop();
};

#endif //TERMU_NES_H
