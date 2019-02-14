package main.java.com.eightbitforest.termu.gui.term.terminal;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.TextCharacter;
import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.screen.TerminalScreen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import com.googlecode.lanterna.terminal.swing.SwingTerminalFontConfiguration;

import java.awt.*;
import java.io.IOException;
import java.util.HashMap;

public class LanternaTerminal implements ITerminal {
    private HashMap<Long, TextCharacter> characterCache;
    private Screen screen;
    private int background;

    public LanternaTerminal() {
        characterCache = new HashMap<>();
    }

    @Override
    public void init() {
        try {
            DefaultTerminalFactory defaultTerminalFactory = new DefaultTerminalFactory();
            defaultTerminalFactory.setTerminalEmulatorFontConfiguration(SwingTerminalFontConfiguration.newInstance(
                    new Font("monospaced", Font.BOLD, 8)
            ));
            defaultTerminalFactory.setInitialTerminalSize(new TerminalSize(256 / 2, 224 / 4));

            screen = new TerminalScreen(defaultTerminalFactory.createTerminal());
            screen.startScreen();
            screen.setCursorPosition(null);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void setChar(char c, int x, int y, int foreground) {
        screen.setCharacter(x, y, getCachedCharacter(c, foreground));
    }

    @Override
    public void setBackground(int background) {
        this.background = background;
        characterCache.clear();
    }

    @Override
    public void refresh() {
        try {
            screen.refresh();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private TextCharacter getCachedCharacter(char c, int foreground) {
        long key = (long) c << 32 | (foreground & 0x00000000ffffffffL);

        if (characterCache.containsKey(key)) {
            return characterCache.get(key);
        } else {
            TextCharacter textCharacter = new TextCharacter(
                    c,
                    new TextColor.RGB(foreground >> 16 & 0xff, foreground >> 8 & 0xff, foreground & 0xff),
                    new TextColor.RGB(background >> 16 & 0xff, background >> 8 & 0xff, background & 0xff)
            );
            characterCache.put(key, textCharacter);
            return textCharacter;
        }
    }
}
