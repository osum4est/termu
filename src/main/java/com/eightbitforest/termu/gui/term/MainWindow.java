package main.java.com.eightbitforest.termu.gui.term;

import main.java.com.eightbitforest.termu.gui.term.terminal.ITerminal;
import main.java.com.eightbitforest.termu.gui.term.terminal.LanternaTerminal;
import main.java.com.eightbitforest.termu.gui.term.terminal.pixelterminal.BraillePixelTerminal;
import main.java.com.eightbitforest.termu.gui.term.terminal.pixelterminal.IPixelTerminal;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

class MainWindow {
    void start() {
        ITerminal terminal = new LanternaTerminal();
        IPixelTerminal pixelTerminal = new BraillePixelTerminal();

        terminal.init();
        pixelTerminal.init(256, 224);

        int[][][] allPixels = new int[1][][];
        try {
            allPixels[0] = getPixels(ImageIO.read(new File("test1.png")));
        } catch (IOException e) {
            e.printStackTrace();
        }

        int frames = 0;
        int i = 0;
        long time = System.nanoTime();
        while (true) {
            for (int x = 0; x < 256; x++) {
                for (int y = 0; y < 224; y++) {
                    pixelTerminal.setPixel(x, y, allPixels[i][x][y]);
                }
            }

            pixelTerminal.render(terminal);
            terminal.refresh();

            i++;
            if (i >= allPixels.length)
                i = 0;

            frames++;
            if (System.nanoTime() - time >= 1e+9){
                System.out.println("FPS: " + frames);
                frames = 0;
                time = System.nanoTime();
            }
        }
    }

    private static int[][] getPixels(BufferedImage image) {
        int w = image.getWidth();
        int h = image.getHeight();

        int[][] pixels = new int[w][h];

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                pixels[x][y] = image.getRGB(x, y);
            }
        }

        return pixels;
    }
}
