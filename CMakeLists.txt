cmake_minimum_required(VERSION 3.10)
project(termu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#set(CMAKE_CXX_FLAGS_DEBUG "-O3")

#find_package(OpenCV REQUIRED)

if (WIN32)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/pdcurses/pdcurses.lib COMMAND nmake -f Makefile.vc WIDE=Y DLL=Y PLATFORM=X86 PDCURSES_SRCDIR=../../src/PDCurses-3.8
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/pdcurses)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/lib)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/src/PDCurses-3.8)
else ()
    add_definitions(-D_XOPEN_SOURCE_EXTENDED)
    set(CURSES_NEED_WIDE TRUE)
    find_package(Curses REQUIRED)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

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
        src/emu/nes/system/cpu.h
        src/emu/nes/system/interrupt_handler.h
        src/emu/nes/palettes/palette.h
        src/emu/nes/palettes/nes_2C02_palette.h)

if (WIN32)
    target_link_libraries(termu ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/pdcurses/pdcurses.lib)

    add_custom_command(TARGET termu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/pdcurses/pdcurses.dll
            ${CMAKE_CURRENT_BINARY_DIR})
else ()
    target_link_libraries(termu ${CURSES_LIBRARIES})
endif ()
