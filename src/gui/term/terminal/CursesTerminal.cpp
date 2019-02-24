#include <clocale>
#include <iostream>
#include "CursesTerminal.h"

void CursesTerminal::Init() {
    setlocale(LC_ALL, "");
    initscr();

    start_color();
    use_default_colors();
    for (int i = 0; i < COLORS; i++) {
        init_pair(i, i, -1);
    }

    currentForeground = -1;

    cbreak();
    noecho();
}

void CursesTerminal::Close() {
    endwin();
}

void CursesTerminal::SetChar(wchar_t c, int x, int y, int foreground) {
    // TODO: Color palettes
    if (currentForeground != foreground)
        attron(COLOR_PAIR(foreground % 256));

    mvaddwstr(y, x, &c);
}

void CursesTerminal::SetBackground(int background) {
    this->background = background;
}

void CursesTerminal::Refresh() {
    refresh();
}
