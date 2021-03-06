/**
 *  Cloudtari
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "ColorTable.h"
#include "TelevisionSDL.h"

TelevisionSDL::TelevisionSDL()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  window = SDL_CreateWindow(
    "Debug",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width,
    height,
    0);

  background_rect.x = 0;
  background_rect.y = 0;
  background_rect.w = width;
  background_rect.h = height;

  // NOTE: It's possible to write directly to the surface, but requires
  // SDL_LockSurface() so it seems this might be a little simpler for now.
  image = (uint32_t *)malloc(width * height * 4);

  //SDL_RenderClear(renderer);
  screen = SDL_GetWindowSurface(window);

  background_color = SDL_MapRGB(screen->format, 50, 50, 100);
}

TelevisionSDL::~TelevisionSDL()
{
  free(image);

  if (screen != NULL) { SDL_FreeSurface(screen); }
  if (window != NULL) { SDL_DestroyWindow(window); }

  SDL_Quit();
}

int TelevisionSDL::init()
{
  return 0;
}

#if 0
void TelevisionSDL::clear_display()
{
  //SDL_RenderClear(renderer);
  SDL_FillRect(screen, &background_rect, background_color);
  SDL_UpdateWindowSurface(window);
}

void TelevisionSDL::draw_pixel(int x, int y, uint32_t color)
{
  SDL_Rect rect;

  rect.x = x * 3;
  rect.y = y * 2;
  rect.w = 3;
  rect.h = 2;

  SDL_FillRect(screen, &rect, color);
}

void TelevisionSDL::draw_pixel(int x, int y, uint8_t color)
{
  SDL_Rect rect;

  rect.x = x * 3;
  rect.y = y * 2;
  rect.w = 3;
  rect.h = 2;

  SDL_FillRect(screen, &rect, ColorTable::get_color(color));
}
#endif

bool TelevisionSDL::refresh()
{
  pause();

#if 0
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = width;
  rect.h = height;
#endif

  // Copy the drawn image to the screen.
  SDL_LockSurface(screen);
  memcpy(screen->pixels, image, width * height * 4);
  SDL_UnlockSurface(screen);

  SDL_UpdateWindowSurface(window);

  return true;
}

int TelevisionSDL::handle_events()
{
  SDL_Event event;

  if (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) { return KEY_QUIT; }
        if (event.key.keysym.sym == SDLK_TAB) { return KEY_SELECT_DOWN; }
        if (event.key.keysym.sym == SDLK_RETURN) { return KEY_RESET_DOWN; }
        if (event.key.keysym.sym == SDLK_LEFT) { return KEY_LEFT_DOWN; }
        if (event.key.keysym.sym == SDLK_RIGHT) { return KEY_RIGHT_DOWN; }
        if (event.key.keysym.sym == SDLK_UP) { return KEY_UP_DOWN; }
        if (event.key.keysym.sym == SDLK_DOWN) { return KEY_DOWN_DOWN; }
        if (event.key.keysym.sym == SDLK_SPACE) { return KEY_FIRE_DOWN; }
        break;

      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_TAB) { return KEY_SELECT_UP; }
        if (event.key.keysym.sym == SDLK_RETURN) { return KEY_RESET_UP; }
        if (event.key.keysym.sym == SDLK_LEFT) { return KEY_LEFT_UP; }
        if (event.key.keysym.sym == SDLK_RIGHT) { return KEY_RIGHT_UP; }
        if (event.key.keysym.sym == SDLK_UP) { return KEY_UP_UP; }
        if (event.key.keysym.sym == SDLK_DOWN) { return KEY_DOWN_UP; }
        if (event.key.keysym.sym == SDLK_SPACE) { return KEY_FIRE_UP; }
        break;

        break;
      case SDL_QUIT:
        return KEY_QUIT;
    }
  }

#if 0
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  //if (keystate[SDLK_ESCAPE])
  if (keystate[SDL_SCANCODE_ESCAPE])
  {
    return -1;
  }
#endif

  return 0;
}

