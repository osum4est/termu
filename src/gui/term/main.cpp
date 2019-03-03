#include <iostream>
#include <thread>
#include "terminal/curses_display.h"
#include "../../emu/core/rom/rom_path.h"
#include "../../emu/core/emulator.h"
#include "../../emu/core/emulator_loader.h"

// TODO: Include guard everything, formatting
// TODO: config.h with DEFINE DEBUG to turn off debug stuff
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Please provide a path to a rom to run." << std::endl;
        exit(-1);
    }

    rom_path rom_path(argv[1]);
    emulator *emulator = emulator_loader::get_emulator_for_rom(rom_path);
    emulator->load_rom(rom_path);

    curses_display display;
	emulator->set_display(&display);

	// TODO: move gfx calculation to own thread
	emulator->start();
}
