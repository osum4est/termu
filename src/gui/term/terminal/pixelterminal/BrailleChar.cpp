#include "BrailleChar.h"

const int BrailleChar::UNICODE_OFFSETS[2][4] = {
        {0x01, 0x02, 0x04, 0x40},
        {0x08, 0x10, 0x20, 0x80}
};

BrailleChar::BrailleChar() {
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 4; y++) {
            colors[x][y] = 0;
            currentChar[x][y] = false;
        }
    }
}

void BrailleChar::SetPixel(int x, int y, int color) {
    if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT)
        return;

    if (currentChar[x][y]) {
        int oldColor = colors[x][y];

        // Only update if needed
        if (oldColor == color)
            return;

        maxColor.Decrease(oldColor);
    }

    maxColor.Increase(color);
    colors[x][y] = color;
    currentChar[x][y] = true;
}

void BrailleChar::Update(int backgroundIgnoreColor) {
    color = maxColor.GetMaxObject(backgroundIgnoreColor, backgroundIgnoreColor);

    brailleChar = (wchar_t) UNICODE_START;
    for (int y = 0; y < CHAR_HEIGHT; y++) {
        for (int x = 0; x < CHAR_WIDTH; x++) {
            if (!currentChar[x][y] || colors[x][y] != color)
                continue;

            brailleChar += UNICODE_OFFSETS[x][y];
        }
    }
}

int BrailleChar::GetColor() {
    return color;
}

wchar_t BrailleChar::GetChar() {
    return brailleChar;
}

bool BrailleChar::IsPixelSet(int x, int y) {
    if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT)
        return false;

    return currentChar[x][y];
}

int BrailleChar::GetPixelColor(int x, int y) {
    if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT)
        return 0;

    return colors[x][y];
}
