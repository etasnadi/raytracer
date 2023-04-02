#ifndef DISP_SDL_H
#define DISP_SDL_H

#include "renderer.h"

#include <SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL_image.h>

#include <queue>

class RenderingCanvas {
public:
  SDL_Texture *sdlTexture;
  SDL_Renderer *sdlRenderer;
  int mWidth;
  int mHeight;
  void *mRawPixels;
  int mRawPitch;
  RenderingCanvas(int width, int height, SDL_Renderer *sdlRenderer);
  ~RenderingCanvas();
  void render();
  void lock();
  void unLock();
};

class Display {
public:
  static bool initSDL();
  static void destroySDL();

  int texWidth;
  int texHeight;

  std::deque<int> frameTimeStapms;
  int fpsStatsMovinWindowSize = 10;
  RenderingCanvas *renderingCanvas;
  SDL_Window *gWindow = NULL;
  SDL_Renderer *gRenderer = NULL;
  raytracer_cu::Renderer *renderer;

  Display(int screenW, int screenH);
  bool loadUserTexture(std::string path);
  void mainLoop();
  ~Display();
};

#endif
