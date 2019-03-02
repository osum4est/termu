#include <iostream>
#include <thread>
#include "terminal/curses_display.h"
#include "../../emu/core/rom/rom_path.h"
#include "../../emu/core/emulator.h"
#include "../../emu/core/emulator_loader.h"

// TODO: Include guard everything, formatting
// TODO: config.h with DEFINE DEBUG to turn off debug stuff
int main() {
	curses_display display;

//    rom_path rom_path("roms/test1.nes");
    rom_path rom_path("roms/test2.nes");
//     rom_path rom_path("roms/nestest.nes");

    emulator *emulator = emulator_loader::get_emulator_for_rom(rom_path);
    emulator->load_rom(rom_path);
	emulator->set_display(&display);

	// TODO: move gfx calculation to own thread
    std::cout << "Starting " << emulator->get_loaded_rom()->get_name() << "." << std::endl;
	emulator->start();
}
