#include "InputManager.h"

InputManager::InputManager()
    : window(nullptr), slash(nullptr), newGameButton(nullptr), gameOverButton(nullptr),
      newGameOrigW(0), newGameOrigH(0), gameOverOrigW(0), gameOverOrigH(0) {}

void InputManager::initialize(SDL_Window* window_, Slash& slash_, Button& newGameButton_, Button& gameOverButton_) {
    window = window_;
    slash = &slash_;
    newGameButton = &newGameButton_;
    gameOverButton = &gameOverButton_;
    newGameOrigW = newGameButton->rect.w / 1.1;
    newGameOrigH = newGameButton->rect.h / 1.1;
    gameOverOrigW = gameOverButton->rect.w / std::min(0.5f, std::min(SCREEN_WIDTH * 0.5f / gameOverButton->rect.w, SCREEN_HEIGHT * 0.5f / gameOverButton->rect.h));
    gameOverOrigH = gameOverButton->rect.h / std::min(0.5f, std::min(SCREEN_WIDTH * 0.5f / gameOverButton->rect.w, SCREEN_HEIGHT * 0.5f / gameOverButton->rect.h));
}

bool InputManager::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            updateWindowSize(event.window.data1, event.window.data2);
        } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;
            slash->active = true;
            slash->startTime = SDL_GetTicks();
            slash->lastPointTime = slash->startTime;
            slash->lastMotionTime = slash->startTime;
            slash->points.clear();
            SlashPoint point = {static_cast<float>(mx), static_cast<float>(my), slash->startTime, 0.0};
            slash->points.push_back(point);
        } else if (event.type == SDL_MOUSEMOTION && slash->active) {
            int mx = event.motion.x;
            int my = event.motion.y;
            slash->lastMotionTime = SDL_GetTicks();
            if (SDL_GetTicks() - slash->lastPointTime >= 5) {
                if (!slash->points.empty()) {
                    SlashPoint lastPoint = slash->points.back();
                    float dx = mx - lastPoint.x;
                    float dy = my - lastPoint.y;
                    float distance = sqrt(dx * dx + dy * dy);
                    if (distance >= 1.0f) {
                        double angle = atan2(dy, dx);
                        SlashPoint point = {static_cast<float>(mx), static_cast<float>(my), SDL_GetTicks(), angle};
                        slash->points.push_back(point);
                        slash->lastPointTime = SDL_GetTicks();
                        trimSlashToMaxLength(*slash);
                    }
                } else {
                    SlashPoint point = {static_cast<float>(mx), static_cast<float>(my), SDL_GetTicks(), 0.0};
                    slash->points.push_back(point);
                    slash->lastPointTime = SDL_GetTicks();
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            slash->active = false;
            slash->endTime = SDL_GetTicks();
            slash->points.clear();
            *slash = {{}, false, 0, 0, 0, 0};
        }
    }
    return true;
}

void InputManager::updateWindowSize(int newWidth, int newHeight) {
    SCREEN_WIDTH = newWidth;
    SCREEN_HEIGHT = newHeight;
    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    newGameButton->rect.w = static_cast<int>(newGameOrigW * 1.1);
    newGameButton->rect.h = static_cast<int>(newGameOrigH * 1.1);
    newGameButton->rect.x = static_cast<int>(SCREEN_WIDTH * 0.75 - newGameButton->rect.w / 2);
    newGameButton->rect.y = static_cast<int>(SCREEN_HEIGHT * 0.75 - newGameButton->rect.h / 2);
    newGameButton->center = {newGameButton->rect.w / 2, newGameButton->rect.h / 2};
    float scale = std::min(0.5f, std::min(SCREEN_WIDTH * 0.5f / gameOverOrigW, SCREEN_HEIGHT * 0.5f / gameOverOrigH));
    gameOverButton->rect.w = static_cast<int>(gameOverOrigW * scale);
    gameOverButton->rect.h = static_cast<int>(gameOverOrigH * scale);
    gameOverButton->rect.x = (SCREEN_WIDTH - gameOverButton->rect.w) / 2;
    gameOverButton->rect.y = (SCREEN_HEIGHT - gameOverButton->rect.h) / 2;
    gameOverButton->center = {gameOverButton->rect.w / 2, newGameButton->rect.h / 2};
}

void InputManager::trimSlashToMaxLength(Slash& slash) {
    while (slash.points.size() > 1) {
        float totalLength = 0.0f;
        for (size_t i = 1; i < slash.points.size(); ++i) {
            const SlashPoint& p1 = slash.points[i - 1];
            const SlashPoint& p2 = slash.points[i];
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            totalLength += sqrt(dx * dx + dy * dy);
        }
        if (totalLength <= MAX_SLASH_LENGTH) break;
        slash.points.erase(slash.points.begin());
    }
}