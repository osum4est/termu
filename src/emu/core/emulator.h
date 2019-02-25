//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_EMULATOR_H
#define TERMU_EMULATOR_H

#include <string>
#include "rom.h"
#include "rom_path.h"

class emulator {
public:
    virtual bool extension_matches(const std::string &extension) = 0;

    virtual rom *load_rom(const rom_path &rom_path) = 0;

    virtual rom *get_loaded_rom() = 0;

    virtual void start() = 0;
};

#endif //TERMU_EMULATOR_H
