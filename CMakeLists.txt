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
        src/gui/term/Main.cpp
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
        )

target_link_libraries(termu ${CURSES_LIBRARIES})
target_link_libraries(termu ${OpenCV_LIBS})
