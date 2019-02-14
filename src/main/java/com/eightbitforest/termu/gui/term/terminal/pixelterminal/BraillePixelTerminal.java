package main.java.com.eightbitforest.termu.gui.term.terminal.pixelterminal;

import main.java.com.eightbitforest.termu.gui.term.terminal.ITerminal;
import main.java.com.eightbitforest.termu.gui.term.utils.MaxAmount;

public class BraillePixelTerminal implements IPixelTerminal {

    private MaxAmount<Integer> maxColor;

    private int charWidth;
    private int charHeight;
    private BrailleChar[][] chars;

    public BraillePixelTerminal() {
        maxColor = new MaxAmount<>();
    }

    @Override
    public void init(int width, int height) {
        charWidth = width / BrailleChar.CHAR_WIDTH;
        charHeight = height / BrailleChar.CHAR_HEIGHT;

        chars = new BrailleChar[charWidth][charHeight];
        for (int x = 0; x < charWidth; x++) {
            for (int y = 0; y < charHeight; y++) {
                chars[x][y] = new BrailleChar();
            }
        }

        // TODO: Set terminal size?
    }

    @Override
    public void setPixel(int x, int y, int color) {
        int charX = x / BrailleChar.CHAR_WIDTH;
        int charY = y / BrailleChar.CHAR_HEIGHT;
        int pixelX = x % BrailleChar.CHAR_WIDTH;
        int pixelY = y % BrailleChar.CHAR_HEIGHT;

        if (charX < 0 || charX >= charWidth || charY < 0 || charY >= charHeight)
            return;

        if (chars[charX][charY].isPixelSet(pixelX, pixelY)) {
            int oldColor = chars[charX][charY].getPixelColor(pixelX, pixelY);

            // Only update if needed
            if (oldColor == color)
                return;

            maxColor.decrease(oldColor);
        }

        maxColor.increase(color);
        chars[charX][charY].setPixel(pixelX, pixelY, color);
    }

    @Override
    public void render(ITerminal terminal) {
        int backgroundIgnoreColor = maxColor.getMaxObject(0);
        terminal.setBackground(0x040404);

        for (int x = 0; x < charWidth; x++) {
            for (int y = 0; y < charHeight; y++) {
                chars[x][y].update(backgroundIgnoreColor);
                terminal.setChar(chars[x][y].getChar(), x, y, chars[x][y].getColor());
            }
        }
    }
}
