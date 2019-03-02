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

	braille_display.init(width, height);
	braille_display.get_size(&this->width, &this->height);
	framebuffer = braille_display.get_chars();

	background = 0x040404;
	set_color(0, background);
}

void curses_display::close() {
    endwin();
}

void curses_display::set_palette(int *colors, int len) {
    for (int i = 0; i < len; i++) {
		set_color(i + 1, colors[i]);
        init_pair(i + 1, i + 1, 0);
    }
}

void curses_display::set_pixel(int x, int y, int color_idx) {
	braille_display.set_pixel(x, y, color_idx);
}

void curses_display::render() {
	braille_display.update();
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			braille_display::color_char &c_char = framebuffer[y * width + x];
            attron(COLOR_PAIR(c_char.color + 1));
			wchar_t c = c_char.character;
			mvaddwstr(y, x, &c);
		}
	}
	refresh();
}

void curses_display::set_color(int idx, int color) {
	double r = (color >> 16) & 0xff;
	double g = (color >> 8) & 0xff;
	double b = color & 0xff;
	init_color(idx, r / 255 * 1000, g / 255 * 1000, b / 255 * 1000);
}
