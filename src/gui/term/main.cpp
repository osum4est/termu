#include <iostream>
#include "windows/MainWindow.h"
#include "../../emu/core/rom_path.h"
#include "../../emu/core/emulator.h"
#include "../../emu/core/emulator_loader.h"

// TODO: Include guard everything, formatting
// TODO: config.h with DEFINE DEBUG to turn off debug stuff
int main() {

    rom_path rom_path("roms/test1.nes");
//    rom_path rom_path("roms/nestest.nes");
    emulator *emulator = emulator_loader::get_emulator_for_rom(rom_path);

    emulator->load_rom(rom_path);

    std::cout << "Starting " << emulator->get_loaded_rom()->get_name() << "." << std::endl;
    emulator->start();

    // TODO: Re-enable when ready for graphics
//        MainWindow* mainWindow = new MainWindow();
//        mainWindow->Start();
}
