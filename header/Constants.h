#ifndef CONSTANTS_H
#define CONSTANTS_H

#define M_PI 3.14159265358979323846

// Screen dimensions
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// Game constants
const float MAX_SLASH_LENGTH = 450.0f;
const Uint32 SLASH_ACTIVE_DURATION = 100;
const Uint32 SPLASH_FADE_DURATION = 2000;
const Uint32 SPLASH_SCALE_DURATION = 500;
const Uint32 MISS_FADE_DURATION = 1000;
const Uint32 GAME_OVER_DURATION = 3000;
const Uint32 TRANSITION_DURATION = 500;
const float CURVATURE_FACTOR = 0.2f;
const int FRUIT_SIZE = 90;
const float SLASH_TAIL_SPEED = 600.0f;
const Uint32 SLASH_POINT_LIFETIME = 75;
const float HITBOX_RADIUS = 0.9f * (FRUIT_SIZE / 2);
const float FRUIT_GRAVITY = 375.0f;
const float BOMB_GRAVITY = 600.0f;
const float MIN_SPAWN_SEPARATION = 100.0f;

#endif // CONSTANTS_H