#include <clocale>
#include <iostream>
#include "curses_display.h"
#include "../exceptions/gui_exception.h"

void curses_display::init(int width, int height) {
    setlocale(LC_ALL, "");
    initscr();

    if (!has_colors())
        throw gui_exception("This terminal does not support colors!");

    if (!can_change_color())
        throw gui_exception("This terminal cannot change colors!");

    start_color();
    use_default_colors();

    cbreak();
    noecho();
    curs_set(0);

    braille.init(width, height);
    braille.get_size(&this->width, &this->height);
    framebuffer = braille.get_chars();

    game_window = newwin(this->height + 2, this->width + 2, 1, 2);
    box(game_window, 0, 0);

    set_color(16, 0);
}

void curses_display::close() {
    endwin();
}

void curses_display::set_palette(int *colors, int len) {
    for (int i = 0; i < len; i++) {
        set_color(i + 17, colors[i]);
        init_pair((short) (i + 17), (short) (i + 17), 0);
    }
}

void curses_display::set_pixel(int x, int y, int color_idx) {
    braille.set_pixel(x, y, color_idx);
}

void curses_display::render() {
    braille.update();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            braille_display::color_char &c_char = framebuffer[y * width + x];
            wattron(game_window, COLOR_PAIR(c_char.color + 17));
            wchar_t c = c_char.character;
            mvwaddwstr(game_window, y + 1, x + 1, &c);
        }
    }
    wrefresh(game_window);
}

void curses_display::set_color(int idx, int color) {
    double r = (color >> 16) & 0xff;
    double g = (color >> 8) & 0xff;
    double b = color & 0xff;
    init_color((short) idx, (short) (r / 255 * 1000), (short) (g / 255 * 1000), (short) (b / 255 * 1000));
}
