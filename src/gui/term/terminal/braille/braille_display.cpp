#include <iostream>
#include "braille_display.h"

braille_display::~braille_display() {
	if (chars != nullptr) {
		for (int i = 0; i < charWidth; i++)
			delete[] chars[i];
		delete[] chars;
	}
	if (color_chars != nullptr)
		delete[] color_chars;
}

void braille_display::init(int width, int height) {
    charWidth = width / BrailleChar::CHAR_WIDTH;
    charHeight = height / BrailleChar::CHAR_HEIGHT;

    chars = new BrailleChar*[charWidth];
    for (int x = 0; x < charWidth; x++) {
        chars[x] = new BrailleChar[charHeight];
        for (int y = 0; y < charHeight; y++) {
            chars[x][y] = BrailleChar();
        }
    }

	color_chars = new color_char[charWidth * charHeight];
}

void braille_display::set_pixel(int x, int y, int color) {
    int charX = x / BrailleChar::CHAR_WIDTH;
    int charY = y / BrailleChar::CHAR_HEIGHT;
    int pixelX = x % BrailleChar::CHAR_WIDTH;
    int pixelY = y % BrailleChar::CHAR_HEIGHT;

    if (charX < 0 || charX >= charWidth || charY < 0 || charY >= charHeight)
        return;

    if (chars[charX][charY].IsPixelSet(pixelX, pixelY)) {
        int oldColor = chars[charX][charY].GetPixelColor(pixelX, pixelY);

        // Only update if needed
        if (oldColor == color)
            return;

        maxColor.Decrease(oldColor);
    }

    maxColor.Increase(color);
    chars[charX][charY].SetPixel(pixelX, pixelY, color);
}

void braille_display::update() {
    int backgroundIgnoreColor = maxColor.GetMaxObject(0);

    for (int y = 0; y < charHeight; y++) {
        for (int x = 0; x < charWidth; x++) {
            chars[x][y].Update(backgroundIgnoreColor);
			color_chars[y * charWidth + x].character = chars[x][y].GetChar();
			color_chars[y * charWidth + x].color = chars[x][y].GetColor();
        }
    }
}

void braille_display::get_size(int *width, int *height) {
	*width = charWidth;
	*height = charHeight;
}

braille_display::color_char *braille_display::get_chars() {
	return color_chars;
}
