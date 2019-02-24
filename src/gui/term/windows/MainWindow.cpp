#include "MainWindow.h"
#include "../terminal/CursesTerminal.h"
#include "../terminal/pixelterminal/BraillePixelTerminal.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

void MainWindow::Start() {
    Terminal* terminal = new CursesTerminal();
    PixelTerminal* pixelTerminal = new BraillePixelTerminal();

    terminal->Init();
    pixelTerminal->Init(256, 224);

    int numImages = 2;
    int ***allPixels = new int **[numImages];

    allPixels[0] = GetPixels("./test1.png");
    allPixels[1] = GetPixels("./test2.png");

    int frames = 0;
    int i = 0;

    auto time = std::chrono::high_resolution_clock::now();
    while (true) {
        for (int x = 0; x < 256; x++) {
            for (int y = 0; y < 224; y++) {
                pixelTerminal->SetPixel(x, y, allPixels[i][x][y]);
            }
        }

        pixelTerminal->Render(terminal);
        terminal->Refresh();

        i++;
        if (i >= numImages)
            i = 0;

        frames++;
        if (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - time).count() >= 1) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            time = std::chrono::high_resolution_clock::now();
        }
    }

    terminal->Close();
}

int **MainWindow::GetPixels(std::string file) {
    cv::Mat image;
    image = cv::imread(file);

    int w = image.cols;
    int h = image.rows;

    int **pixels = new int *[w];

    for (int x = 0; x < w; x++) {
        pixels[x] = new int[h];
        for (int y = 0; y < h; y++) {
            cv::Vec3b rgb = image.at<cv::Vec3b>(y, x);
            pixels[x][y] = (rgb[0] << 16) | (rgb[1] << 8) | (rgb[2]);
        }
    }

    return pixels;
}

