cmake_minimum_required(VERSION 3.10)
project(termu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#set(CMAKE_CXX_FLAGS_DEBUG "-O3")

# curses
add_definitions(-D_XOPEN_SOURCE_EXTENDED)
find_package(Curses REQUIRED)

# spdlog
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include)

# libuiohook
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/libuiohook/.libs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/libuiohook/include)

# rtaudio
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/rtaudio)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/rtaudio)

add_executable(termu
        src/gui/term/main.cpp
        src/utils/utils.h
        src/gui/term/utils/MaxAmount.h
        src/gui/term/terminal/curses_display.cpp
        src/gui/term/terminal/curses_display.h
        src/gui/term/terminal/braille/BrailleChar.cpp
        src/gui/term/terminal/braille/BrailleChar.h
        src/gui/term/terminal/braille/braille_display.h
        src/gui/term/terminal/braille/braille_display.cpp
        src/emu/core/exceptions/emu_exception.h
        src/emu/core/exceptions/rom_load_exception.h
        src/emu/core/emulator_loader.cpp
        src/emu/core/emulator_loader.h
        src/emu/core/emulator.h
        src/emu/core/rom/rom_path.cpp
        src/emu/core/rom/rom_path.h
        src/emu/core/rom/rom.h
        src/emu/nes/exceptions/missing_mapper_exception.h
        src/emu/nes/rom/nes_rom.cpp
        src/emu/nes/rom/nes_rom.h
        src/emu/nes/rom/mappers/mapper.cpp
        src/emu/nes/rom/mappers/mapper.h
        src/emu/nes/rom/mappers/mapper000_nrom.cpp
        src/emu/nes/rom/mappers/mapper000_nrom.h
        src/emu/nes/rom/mappers/mapper001_mmc1.cpp
        src/emu/nes/rom/mappers/mapper001_mmc1.h
        src/emu/nes/nes_emulator.cpp
        src/emu/nes/nes_emulator.h
        src/emu/nes/system/ppu.cpp
        src/emu/nes/system/ppu.h
        src/emu/nes/system/mem.cpp
        src/emu/nes/system/mem.h
        src/emu/nes/system/cpu.cpp
        src/emu/nes/system/cpu.h
        src/emu/nes/system/interrupt_handler.h
        src/emu/nes/palettes/palette.h
        src/emu/nes/palettes/nes_2C02_palette.h
        src/emu/core/input/emu_controller.h
        src/gui/term/input/keyboard_controller.cpp
        src/gui/term/input/keyboard_controller.h
        src/emu/nes/input/nes_input_device.h
        src/emu/nes/input/nes_controller.cpp
        src/emu/nes/input/nes_controller.h
        src/emu/nes/system/apu.cpp
        src/emu/nes/system/apu.h
		src/emu/nes/system/sound/apu_channel.cpp
		src/emu/nes/system/sound/apu_channel.h
		src/emu/nes/system/sound/triangle_channel.cpp
		src/emu/nes/system/sound/triangle_channel.h
		src/emu/nes/system/sound/pulse_channel.cpp
		src/emu/nes/system/sound/pulse_channel.h
		src/emu/nes/system/sound/apu_envelope_channel.cpp
		src/emu/nes/system/sound/apu_envelope_channel.h src/emu/nes/system/sound/noise_channel.cpp src/emu/nes/system/sound/noise_channel.h)

target_link_libraries(termu ${CURSES_LIBRARIES})
target_link_libraries(termu pthread)
target_link_libraries(termu uiohook)
target_link_libraries(termu rtaudio)
