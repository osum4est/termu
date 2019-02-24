#ifndef TERMU_TERMINAL_H
#define TERMU_TERMINAL_H

class Terminal {
public:
    virtual void Init() = 0;

    virtual void Close() = 0;

    virtual void SetChar(wchar_t c, int x, int y, int foreground) = 0;

    virtual void SetBackground(int background) = 0;

    virtual void Refresh() = 0;
};

#endif //TERMU_TERMINAL_H
