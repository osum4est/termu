cmake_minimum_required(VERSION 3.12)
project(termu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS_DEBUG "-O3")

add_definitions(-D_XOPEN_SOURCE_EXTENDED)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
find_package(OpenCV REQUIRED)


add_executable(termu
        src/gui/term/main.cpp
        src/utils/utils.h
        src/gui/term/windows/MainWindow.cpp
        src/gui/term/windows/MainWindow.h
        src/gui/term/utils/MaxAmount.h
        src/gui/term/terminal/Terminal.h
        src/gui/term/terminal/CursesTerminal.cpp
        src/gui/term/terminal/CursesTerminal.h
        src/gui/term/terminal/pixelterminal/PixelTerminal.h
        src/gui/term/terminal/pixelterminal/BrailleChar.cpp
        src/gui/term/terminal/pixelterminal/BrailleChar.h
        src/gui/term/terminal/pixelterminal/BrailleChar.h
        src/gui/term/terminal/pixelterminal/BraillePixelTerminal.cpp
        src/gui/term/terminal/pixelterminal/BraillePixelTerminal.h
        src/emu/core/exceptions/emu_exception.h
        src/emu/core/exceptions/rom_load_exception.h
        src/emu/core/emulator_loader.cpp
        src/emu/core/emulator_loader.h
        src/emu/core/emulator.h
        src/emu/core/rom_path.cpp
        src/emu/core/rom_path.h src/emu/core/rom.h
        src/emu/nes/exceptions/missing_mapper_exception.h
        src/emu/nes/rom/nes_rom.cpp
        src/emu/nes/rom/nes_rom.h
        src/emu/nes/rom/mappers/mapper.cpp
        src/emu/nes/rom/mappers/mapper.h
        src/emu/nes/rom/mappers/mapper0000.cpp
        src/emu/nes/rom/mappers/mapper0000.h
        src/emu/nes/nes_emulator.cpp
        src/emu/nes/nes_emulator.h
        src/emu/nes/system/nes.cpp
        src/emu/nes/system/nes.h
        src/emu/nes/system/ppu.cpp
        src/emu/nes/system/ppu.h
        src/emu/nes/system/mem.cpp
        src/emu/nes/system/mem.h
        src/emu/nes/system/cpu.cpp
        src/emu/nes/system/cpu.h)

target_link_libraries(termu ${CURSES_LIBRARIES})
target_link_libraries(termu ${OpenCV_LIBS})
