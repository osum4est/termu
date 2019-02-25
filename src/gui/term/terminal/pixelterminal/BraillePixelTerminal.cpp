#include <iostream>
#include "BraillePixelTerminal.h"

void BraillePixelTerminal::Init(int width, int height) {
    charWidth = width / BrailleChar::CHAR_WIDTH;
    charHeight = height / BrailleChar::CHAR_HEIGHT;

    chars = new BrailleChar*[charWidth];
    for (int x = 0; x < charWidth; x++) {
        chars[x] = new BrailleChar[charHeight];
        for (int y = 0; y < charHeight; y++) {
            chars[x][y] = BrailleChar();
        }
    }

    // TODO: Set terminal size?
}

void BraillePixelTerminal::SetPixel(int x, int y, int color) {
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

void BraillePixelTerminal::Render(Terminal* terminal) {
    int backgroundIgnoreColor = maxColor.GetMaxObject(0);
    terminal->SetBackground(0x040404);

    for (int x = 0; x < charWidth; x++) {
        for (int y = 0; y < charHeight; y++) {
            chars[x][y].Update(backgroundIgnoreColor);
            terminal->SetChar(chars[x][y].GetChar(), x, y, chars[x][y].GetColor());
        }
    }
}
