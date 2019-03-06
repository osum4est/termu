//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_NESEMULATOR_H
#define TERMU_NESEMULATOR_H

#include "rom/nes_rom.h"
#include "../core/emulator.h"
#include "system/cpu.h"

/**
 * HUGE thanks to <a href="wiki.nesdev.com">wiki.nesdev.com</a> for providing detailed information on all the inner workings of the NES.
 */
class nes_emulator : public emulator {
    nes_rom *cartridge = nullptr;;
    emu_display *display = nullptr;
    nes_input_device *inputs[2] = {nullptr, nullptr};
    ::cpu *cpu = nullptr;
    ::ppu *ppu = nullptr;
    ::mem *mem = nullptr;

    bool started = false;

public:
    ~nes_emulator();

    bool extension_matches(const std::string &extension) override;

    ::rom *load_rom(const rom_path &rom_path) override;

    ::rom *get_loaded_rom() override;

    void set_display(emu_display *display) override;

    void set_controller(int index, emu_controller *controller) override;

    void start() override;

    void stop() override;
};

#endif //TERMU_NESEMULATOR_H
