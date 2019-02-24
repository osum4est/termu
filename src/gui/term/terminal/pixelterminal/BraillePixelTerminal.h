#include "PixelTerminal.h"
#include "BrailleChar.h"
#include "../../utils/MaxAmount.h"

class BraillePixelTerminal : public PixelTerminal {
    MaxAmount<int> maxColor;

    int charWidth;
    int charHeight;
    BrailleChar **chars;

public:
    void Init(int width, int height) override;

    void SetPixel(int x, int y, int color) override;

    void Render(Terminal* terminal) override;
};
