//
// Created by Forrest Jones on 2019-02-23.
//

#include <string>

class MainWindow {
public:
    void Start();

private:
    static int **GetPixels(std::string file);
};
