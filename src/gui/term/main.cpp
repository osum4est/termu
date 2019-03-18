#include <iostream>
#include <thread>
#include <uiohook.h>
#include "terminal/curses_display.h"
#include "../../emu/core/rom/rom_path.h"
#include "../../emu/core/emulator.h"
#include "../../emu/core/emulator_loader.h"
#include "input/keyboard_controller.h"

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

    // keyboard_controller controller({VC_X, VC_Z, VC_S, VC_A, VC_UP, VC_DOWN, VC_LEFT, VC_RIGHT, VC_Q});
    keyboard_controller controller({VC_X, VC_Z, VC_S, VC_A, VC_K, VC_J, VC_H, VC_L, VC_Q});
    emulator->set_controller(0, &controller);

    // TODO: move gfx calculation to own thread

    std::thread emu_thread([&emulator]() {
        emulator->start();
    });

    keyboard_controller::poll_input(&controller); // libuiohook doesn't seem to work if it's not the main thread

	// If poll_input returns, then the user requested to quit
	emulator->stop();
	emu_thread.join();
	display.close();
}
