package main.java.com.eightbitforest.termu.gui.term.terminal;

public interface ITerminal {
    void init();

    void setChar(char c, int x, int y, int foreground);

    void setBackground(int background);

    void refresh();
}
