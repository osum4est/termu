//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_EMULATORLOADER_H
#define TERMU_EMULATORLOADER_H

#include <vector>
#include "emulator.h"

class emulator_loader {
private:
    static const std::vector<emulator *> emulators;

public:
    static emulator *get_emulator_for_rom(const rom_path &rom_path);
};

#endif //TERMU_EMULATORLOADER_H
