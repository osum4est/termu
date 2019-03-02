//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_EMUDISPLAY_H
#define TERMU_EMUDISPLAY_H

class emu_display {
public:
	virtual void init(int width, int height) = 0;
	virtual void set_palette(int* colors, int len) = 0;
	virtual void set_pixel(int x, int y, int color_idx) = 0;
	virtual void render() = 0;
};

#endif //TERMU_EMUDISPLAY_H