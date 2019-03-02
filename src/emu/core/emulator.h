//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_EMULATOR_H
#define TERMU_EMULATOR_H

#include <string>
#include "rom/rom.h"
#include "rom/rom_path.h"
#include "display/emu_display.h"

class emulator {
public:
    virtual bool extension_matches(const std::string &extension) = 0;

    virtual rom *load_rom(const rom_path &rom_path) = 0;

    virtual rom *get_loaded_rom() = 0;

	virtual void set_display(emu_display *display) = 0;

    virtual void start() = 0;
};

#endif //TERMU_EMULATOR_H
