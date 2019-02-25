#include "emulator_loader.h"
#include "exceptions/rom_load_exception.h"
#include "../nes/nes_emulator.h"

const std::vector<emulator *> emulator_loader::emulators = {
        (emulator *) new nes_emulator()
};


emulator *emulator_loader::get_emulator_for_rom(const rom_path &rom_path) {
    emulator *emulator = nullptr;

    for (::emulator *emu : emulators) {
        if (emu->extension_matches(rom_path.get_extension()))
            emulator = emu;
    }

    if (emulator == nullptr)
        throw rom_load_exception("Could not find emulator for extension: " + rom_path.get_extension() + ".");

    return emulator;
}
