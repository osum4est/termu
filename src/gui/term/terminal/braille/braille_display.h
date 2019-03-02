#include "BrailleChar.h"
#include "../../utils/MaxAmount.h"

class braille_display {
public:
	struct color_char
	{
		wchar_t character = 0;
		int color = 0;
	};

private:
    MaxAmount<int> maxColor;
    int charWidth;
    int charHeight;
    BrailleChar **chars = nullptr;
    color_char *color_chars = nullptr;

public:
	~braille_display();
    void init(int width, int height);

    void set_pixel(int x, int y, int color);

	void update();

	void get_size(int *width, int *height);

	color_char *get_chars();
};
