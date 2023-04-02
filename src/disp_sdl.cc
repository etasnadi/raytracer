#include "disp_sdl.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL_image.h>

#include "basic_types.h"
#include "cuda_runtime.h"

RenderingCanvas::RenderingCanvas(int width, int height,
                                 SDL_Renderer *a_sdlRenderer) {
  sdlRenderer = a_sdlRenderer;
  mWidth = width;
  mHeight = height;
  sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
                               SDL_TEXTUREACCESS_STREAMING, width, height);
}

RenderingCanvas::~RenderingCanvas() { SDL_DestroyTexture(sdlTexture); }

void RenderingCanvas::lock() {
  SDL_LockTexture(sdlTexture, NULL, &mRawPixels, &mRawPitch);
}

void RenderingCanvas::unLock() { SDL_UnlockTexture(sdlTexture); }

void RenderingCanvas::render() {
  SDL_Rect renderQuad = {0, 0, mWidth, mHeight};
  SDL_RenderCopyEx(sdlRenderer, sdlTexture, NULL, &renderQuad, 0.0f, NULL,
                   SDL_FLIP_NONE);
}

Display::~Display() {
  delete renderer;
  delete renderingCanvas;

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;
}

bool Display::initSDL() {
  bool success = true;
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }

    // Seed random
    srand(SDL_GetTicks());
  }
  return success;
}

void Display::destroySDL() {
  // Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

bool Display::loadUserTexture(std::string path) {
  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    return false;
  }

  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  std::cout << loadedSurface->w << std::endl;

  int bytesPerPixel = loadedSurface->pitch / loadedSurface->w;
  std::cout << "Bytes per pixel: " << bytesPerPixel << std::endl;

  uint8_t *pixels = (uint8_t *)loadedSurface->pixels;
  float3 *targetTexturePixels = new float3[loadedSurface->w * loadedSurface->h];

  for (int x = 0; x < loadedSurface->w; x++) {
    for (int y = 0; y < loadedSurface->h; y++) {
      int linIdx = y * (loadedSurface->w) + x;
      uint8_t r = pixels[linIdx * bytesPerPixel + 0];
      uint8_t g = pixels[linIdx * bytesPerPixel + 1];
      uint8_t b = pixels[linIdx * bytesPerPixel + 2];
      targetTexturePixels[linIdx] =
          make_float3(r / 255.0f, g / 255.0f, b / 255.0f);
    }
  }

  renderer->addTexture(loadedSurface->w, loadedSurface->h, targetTexturePixels);

  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
           IMG_GetError());
    return false;
  }
  SDL_FreeSurface(loadedSurface);
  free(targetTexturePixels);
  return true;
}

Display::Display(int screenW, int screenH) {
  bool success = true;

  texWidth = screenW;
  texHeight = screenH;
  renderer = new raytracer_cu::Renderer(screenW, screenH);
  loadUserTexture("../assets/floor.png");
  loadUserTexture("../assets/wall.jpg");
  loadUserTexture("../assets/ceiling.jpg");
  renderer->buildScene();

  // Create window
  gWindow = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, screenW, screenH,
                             SDL_WINDOW_SHOWN);
  if (gWindow == NULL) {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    // Create renderer for window
    gRenderer = SDL_CreateRenderer(
        gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL) {
      printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      // Initialize renderer color
      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

      // Initialize PNG loading
      int imgFlags = IMG_INIT_PNG;
      if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n",
               IMG_GetError());
        success = false;
      }
    }
  }

  renderingCanvas = new RenderingCanvas(screenW, screenH, gRenderer);
}

// Key press surfaces constants
enum KeyPressSurfaces {
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,
  KEY_PRESS_SURFACE_TOTAL
};

void Display::mainLoop() {
  // Main loop flag
  bool quit = false;

  // Event handler
  SDL_Event e;

  int epochTime = SDL_GetTicks();

  SDL_SetRelativeMouseMode(SDL_TRUE);

  // While application is running
  while (!quit) {
    // Handle events on queue
    int cumMouseMotionX = 0;
    int cumMouseMotionY = 0;

    int cumWheel = 0;

    int cumKeyX = 0;
    int cumKeyY = 0;

    while (SDL_PollEvent(&e) != 0) {
      // User requests quit
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
          cumKeyY += 1;
          break;

        case SDLK_DOWN:
          cumKeyY -= 1;
          break;

        case SDLK_RIGHT:
          cumKeyX += 1;
          break;

        case SDLK_LEFT:
          cumKeyX -= 1;
          break;
        }

      } else if (e.type == SDL_MOUSEMOTION) {
        cumMouseMotionX += e.motion.xrel;
        cumMouseMotionY += e.motion.yrel;
      } else if (e.type == SDL_MOUSEWHEEL) {
        cumWheel += e.wheel.y;
      }
    }

    if (cumMouseMotionX != 0 || cumMouseMotionY != 0) {
      renderer->mouseMoveInput(cumMouseMotionX, cumMouseMotionY);
    }

    if (cumWheel != 0) {
      renderer->mouseWheelInput(cumWheel);
    }

    if (cumKeyX != 0 || cumKeyY != 0) {
      renderer->keyboardArrowsInput(cumKeyX, cumKeyY);
    }

    // Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    renderingCanvas->lock();
    renderer->render((uint8_t *)renderingCanvas->mRawPixels);
    renderingCanvas->unLock();

    // Render frame
    renderingCanvas->render();

    // Update screen
    SDL_RenderPresent(gRenderer);

    int currFrameTime = SDL_GetTicks() - epochTime;
    frameTimeStapms.push_back(currFrameTime);
    int firstFrameTime = frameTimeStapms.front();
    if (frameTimeStapms.size() > fpsStatsMovinWindowSize) {
      frameTimeStapms.pop_front();
    }

    if (frameTimeStapms.size() >= fpsStatsMovinWindowSize) {

      float meanElapsedTime =
          (currFrameTime - firstFrameTime) / fpsStatsMovinWindowSize;
      float currFrameElapsedTime =
          currFrameTime - frameTimeStapms[frameTimeStapms.size() - 2];
      float fps = 1000.0f / meanElapsedTime;
      std::cout << "FPS: " << fps
                << " (current frame render time: " << currFrameElapsedTime
                << "ms)" << std::endl;
    }
  }
}