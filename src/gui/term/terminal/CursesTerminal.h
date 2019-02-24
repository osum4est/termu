#include "Terminal.h"
#include <curses.h>

class CursesTerminal : public Terminal {
    int currentForeground;
    int background;

public:
    void Init() override;

    void Close() override;

    void SetChar(wchar_t c, int x, int y, int foreground) override;

    void SetBackground(int background) override;

    void Refresh() override;
};
