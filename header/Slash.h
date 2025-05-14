#ifndef SLASH_H
#define SLASH_H

#include <SDL2/SDL.h>
#include <vector>
#include "Constants.h"

struct SlashPoint {
    float x, y;
    Uint32 time;
    double angle;
};

struct Slash {
    std::vector<SlashPoint> points;
    bool active;
    Uint32 startTime;
    Uint32 endTime;
    Uint32 lastPointTime;
    Uint32 lastMotionTime;
};

void drawContinuousWhiteLine(SDL_Renderer* renderer, const std::vector<SlashPoint>& points);
float calculateSlashLength(const std::vector<SlashPoint>& points);
void trimSlashToMaxLength(Slash& slash);
void updateSlashTail(Slash& slash, float deltaTime, float mouseX, float mouseY, Uint32 currentTime);

#endif // SLASH_H