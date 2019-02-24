
#include "../../utils/MaxAmount.h"

class BrailleChar {
    static const int UNICODE_START = 0x2800;
    static const int UNICODE_OFFSETS[2][4];

    MaxAmount<int> maxColor;

    int colors[2][4];
    bool currentChar[2][4];

    int color;
    wchar_t brailleChar;

public:
    static const int CHAR_WIDTH = 2;
    static const int CHAR_HEIGHT = 4;

    BrailleChar();

    void SetPixel(int x, int y, int color);

    void Update(int backgroundIgnoreColor);

    int GetColor();

    wchar_t GetChar();

    bool IsPixelSet(int x, int y);

    int GetPixelColor(int x, int y);
};
