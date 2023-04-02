#include <iostream>

#include "disp_sdl.h"

int main(int argc, char* args[]){
    Display::initSDL();
    int screenWidth = 1024;
    int screenHeight = 1024;
    Display display(screenWidth, screenHeight);
    display.mainLoop();
    Display::destroySDL();
    return 0;
}