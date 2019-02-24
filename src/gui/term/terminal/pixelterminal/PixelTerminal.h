#include "../Terminal.h"

class PixelTerminal {
public:
    virtual void Init(int width, int height) = 0;

    virtual void SetPixel(int x, int y, int color) = 0;

    virtual void Render(Terminal* terminal) = 0;
};
