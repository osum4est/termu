package main.java.com.eightbitforest.termu.gui.term.terminal.pixelterminal;

import main.java.com.eightbitforest.termu.gui.term.utils.MaxAmount;

class BrailleChar {

    static final int CHAR_WIDTH = 2;
    static final int CHAR_HEIGHT = 4;

    private static final int unicodeStart = 0x2800;
    private static final int[][] unicodeOffsets = {
            {0x01, 0x02, 0x04, 0x40},
            {0x08, 0x10, 0x20, 0x80}
    };

    private MaxAmount<Integer> maxColor;

    private int[][] colors;
    private boolean[][] currentChar;

    private int color;
    private char brailleChar;


    BrailleChar() {
        maxColor = new MaxAmount<>();
        colors = new int[CHAR_WIDTH][CHAR_HEIGHT];
        currentChar = new boolean[CHAR_WIDTH][CHAR_HEIGHT];
    }

    void setPixel(int x, int y, int color) {
        if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT)
            return;

        if (currentChar[x][y]) {
            int oldColor = colors[x][y];

            // Only update if needed
            if (oldColor == color)
                return;

            maxColor.decrease(oldColor);
        }

        maxColor.increase(color);
        colors[x][y] = color;
        currentChar[x][y] = true;
    }

    void update(int backgroundIgnoreColor) {
        color = maxColor.getMaxObject(backgroundIgnoreColor, backgroundIgnoreColor);

        brailleChar = (char) unicodeStart;
        for (int y = 0; y < CHAR_HEIGHT; y++) {
            for (int x = 0; x < CHAR_WIDTH; x++) {
                if (!currentChar[x][y] || colors[x][y] != color)
                    continue;

                brailleChar += unicodeOffsets[x][y];
            }
        }
    }

    int getColor() {
        return color;
    }

    char getChar() {
        return brailleChar;
    }

    boolean isPixelSet(int x, int y) {
        if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT)
            return false;

        return currentChar[x][y];
    }

    int getPixelColor(int x, int y) {
        if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT)
            return 0;

        return colors[x][y];
    }
}
