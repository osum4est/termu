package main.java.com.eightbitforest.termu.gui.term.terminal.pixelterminal;

import main.java.com.eightbitforest.termu.gui.term.terminal.ITerminal;

public interface IPixelTerminal {
    void init(int width, int height);

    void setPixel(int x, int y, int color);

    void render(ITerminal terminal);
}
