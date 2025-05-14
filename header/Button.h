#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>

struct Button {
    SDL_Rect rect;
    SDL_Texture* texture;
    SDL_Point center;
    double angle;
};

#endif // BUTTON_H