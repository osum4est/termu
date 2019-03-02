//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_NES_H
#define TERMU_NES_H

#include "../rom/nes_rom.h"
#include "ppu.h"
#include "cpu.h"
#include "../../core/display/emu_display.h"

class nes {
	nes_rom *cartridge = nullptr;;
	emu_display *display = nullptr;
    ::cpu *cpu;
    ::ppu *ppu;
    ::mem *mem;

public:
    ~nes();

    void insert_cartridge(nes_rom *rom);

	void set_display(emu_display *display);

    void start();

    void stop();
};

#endif //TERMU_NES_H
