#include <iostream>

#include <QApplication>

#include "display.h"

int main(int argc, char **argv){
    /*
    #pragma omp parallel num_threads(3)
    {
        #pragma omp for
        for(int i = 0; i < 100; i++){        
            std::cout << i << " " << std::flush;
        }
    }
    return 0;
    */
    QApplication app (argc, argv);
    Display display;
    display.queryNextFrame();
    return app.exec();
}