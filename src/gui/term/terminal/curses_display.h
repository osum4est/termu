#ifndef TERMU_CURSES_DISPLAY_H
#define TERMU_CURSES_DISPLAY_H

#include <curses.h>
#include "../../../emu/core/display/emu_display.h"
#include "braille/braille_display.h"

class curses_display : public emu_display {
private:
	braille_display::color_char *framebuffer;
	::braille_display braille;

	int background;
	int width;
	int height;

public:
	void init(int width, int height) override;
    void close();
	void set_palette(int *colors, int len) override;
	void set_pixel(int x, int y, int color_idx) override;
	void render() override;

private:
	void set_color(int idx, int color);
};

#endif //TERMU_CURSES_DISPLAY_H
