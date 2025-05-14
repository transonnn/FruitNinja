#include "Slash.h"
#include <cmath>

void drawContinuousWhiteLine(SDL_Renderer* renderer, const std::vector<SlashPoint>& points) {
    if (points.size() < 2) return;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (size_t i = 1; i < points.size(); ++i) {
        float x1 = points[i - 1].x;
        float y1 = points[i - 1].y;
        float x2 = points[i].x;
        float y2 = points[i].y;
        float dx = x2 - x1;
        float dy = y2 - y1;
        float angle = atan2(dy, dx);
        float offset = 1.0f;
        for (float t = 0; t <= 1.0f; t += 0.01f) {
            float xt = (1 - t) * x1 + t * x2;
            float yt = (1 - t) * y1 + t * y2;
            float xtNext = (1 - (t + 0.01f)) * x1 + (t + 0.01f) * x2;
            float ytNext = (1 - (t + 0.01f)) * y1 + (t + 0.01f) * y2;
            SDL_RenderDrawLine(renderer, static_cast<int>(xt), static_cast<int>(yt),
                               static_cast<int>(xtNext), static_cast<int>(ytNext));
            SDL_RenderDrawLine(renderer,
                               static_cast<int>(xt + cos(angle + M_PI / 2) * offset),
                               static_cast<int>(yt + sin(angle + M_PI / 2) * offset),
                               static_cast<int>(xtNext + cos(angle + M_PI / 2) * offset),
                               static_cast<int>(ytNext + sin(angle + M_PI / 2) * offset));
            SDL_RenderDrawLine(renderer,
                               static_cast<int>(xt + cos(angle - M_PI / 2) * offset),
                               static_cast<int>(yt + sin(angle - M_PI / 2) * offset),
                               static_cast<int>(xtNext + cos(angle - M_PI / 2) * offset),
                               static_cast<int>(ytNext + sin(angle - M_PI / 2) * offset));
        }
    }
}

float calculateSlashLength(const std::vector<SlashPoint>& points) {
    float totalLength = 0.0f;
    for (size_t i = 1; i < points.size(); ++i) {
        const SlashPoint& p1 = points[i - 1];
        const SlashPoint& p2 = points[i];
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        totalLength += sqrt(dx * dx + dy * dy);
    }
    return totalLength;
}

void trimSlashToMaxLength(Slash& slash) {
    while (slash.points.size() > 1) {
        float totalLength = calculateSlashLength(slash.points);
        if (totalLength <= MAX_SLASH_LENGTH) break;
        slash.points.erase(slash.points.begin());
    }
}

void updateSlashTail(Slash& slash, float deltaTime, float mouseX, float mouseY, Uint32 currentTime) {
    if (slash.points.size() < 2) return;
    SlashPoint& tail = slash.points.back();
    float dx = mouseX - tail.x;
    float dy = mouseY - tail.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance >= 1.0f) {
        float directionX = dx / distance;
        float directionY = dy / distance;
        float moveDistance = SLASH_TAIL_SPEED * deltaTime;
        if (moveDistance >= distance) {
            tail.x = mouseX;
            tail.y = mouseY;
        } else {
            tail.x += directionX * moveDistance;
            tail.y += directionY * moveDistance;
        }
    }
    if (slash.points.size() > 1 && currentTime - slash.points.front().time >= SLASH_POINT_LIFETIME) {
        slash.points.erase(slash.points.begin());
    }
}