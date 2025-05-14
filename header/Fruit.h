#ifndef FRUIT_H
#define FRUIT_H

#include <SDL2/SDL.h>
#include <string>
#include "Constants.h"

struct Fruit {
    float x, y;
    float vx, vy;
    bool active;
    bool sliced;
    SDL_Texture* texture;
    SDL_Texture* LeftslicedTexture;
    SDL_Texture* RightslicedTexture;
    SDL_Texture* splashTexture;
    std::string type;
    double rotationAngle;
    float splashX, splashY;
    Uint32 splashStartTime;
    bool splashActive;
    float leftX, leftY;
    float rightX, rightY;
    float leftVx, leftVy;
    float rightVx, rightVy;
    double angleLeft, angleRight;
};

struct MissEffect {
    float x;
    Uint32 startTime;
    bool active;
};

#endif // FRUIT_H