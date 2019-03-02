//
// Created by Forrest Jones on 2019-03-01.
//

#ifndef TERMU_PALETTE_H
#define TERMU_PALETTE_H

class palette {
public:
    /**
     * Returns an array of 64 colors, with the first being color 0.
     */
    virtual void get_palette(int *palette) = 0;

protected:
    /**
     * Helper to convert an array of 64 * 3 separate rgb values to an array of 64 single color ints.
     */
    virtual void rgb_palette_to_int(int *rgb, int *palette) {
        for (int i = 0; i < 64 * 3; i += 3) {
            palette[i / 3] = ((rgb[i] & 0xff) << 16) | ((rgb[i + 1] & 0xff) << 8) | ((rgb[i + 2] & 0xff));
        }
    }
};

#endif //TERMU_PALETTE_H
