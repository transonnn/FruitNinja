#include <bits/stdc++.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int TARGET_RADIUS = 60;
const int GAME_DURATION = 10;
const int MAX_TARGETS = 3;

const int GAME_AREA_WIDTH = 800;
const int GAME_AREA_HEIGHT = 600;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* targetTexture = nullptr;
SDL_Texture* background = nullptr;
TTF_Font* font = nullptr;
Mix_Chunk* hitSound = nullptr;

struct Target {
    int x, y;
};

std::vector<Target> targets;
int score = 0;
int shotsFired = 0;
int shotsHit = 0;
int timeLeft = GAME_DURATION;
Uint32 startTime;

// Vị trí căn giữa của vùng 800x600 trong màn hình 1920x1080
int offsetX = (SCREEN_WIDTH - GAME_AREA_WIDTH) / 2;
int offsetY = (SCREEN_HEIGHT - GAME_AREA_HEIGHT) / 2;

// Hàm load ảnh target từ file
SDL_Texture* loadTexture(const std::string& path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
    if (!newTexture) {
        std::cout << "Failed to load texture! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
    }
    return newTexture;
}

// Khởi tạo SDL
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Aim Trainer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) {
        std::cout << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("E:/Tin/SDL2/LTNC/DemoSDL/project/arial.ttf", 24);
    if (!font) {
        std::cout << "Font Load Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Load ảnh target
    targetTexture = loadTexture("E:/Tin/SDL2/LTNC/DemoSDL/project/target.png");
    if (!targetTexture) return false;

    // Load background
    background = loadTexture("E:/Tin/SDL2/LTNC/DemoSDL/project/background.png");
    if (!background) return false;

    return true;
}

struct Button {
    int x, y, w, h;
    
    bool isInside(int mouseX, int mouseY) {
        return mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h;
    }
};

SDL_Texture* loadTexture(const char* filePath, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(filePath);
    if (!loadedSurface) {
        printf("Failed to load image: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface); // Giải phóng surface sau khi tạo texture

    if (!texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
    }

    return texture;
}

void renderTarget(int x, int y) {
    if (targetTexture) {
        SDL_Rect dstRect = { x - TARGET_RADIUS, y - TARGET_RADIUS, TARGET_RADIUS * 2, TARGET_RADIUS * 2 };
        SDL_RenderCopy(renderer, targetTexture, NULL, &dstRect);
    } else {
        // Nếu không có texture, vẽ hình tròn
        int dx, dy;
        for (int w = 0; w < TARGET_RADIUS * 2; w++) {
            for (int h = 0; h < TARGET_RADIUS * 2; h++) {
                dx = TARGET_RADIUS - w;
                dy = TARGET_RADIUS - h;
                if (dx * dx + dy * dy <= TARGET_RADIUS * TARGET_RADIUS) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                    SDL_RenderDrawPoint(renderer, x - dx, y + dy);
                    SDL_RenderDrawPoint(renderer, x + dx, y - dy);
                    SDL_RenderDrawPoint(renderer, x - dx, y - dy);
                }
            }
        }
    }
}

// Hiển thị text
void renderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Kiểm tra hai targets có trùng nhau không
bool isOverlapping(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    int minDistance = TARGET_RADIUS * 2; // Khoảng cách tối thiểu giữa hai targets
    return (dx * dx + dy * dy) < (minDistance * minDistance);
}

// Tạo target trong vùng 800x600
void spawnTarget() {
    if (targets.size() < MAX_TARGETS) {
        Target newTarget;
        bool validPosition;

        do {
            validPosition = true;
            newTarget.x = offsetX + (rand() % (GAME_AREA_WIDTH - 2 * TARGET_RADIUS) + TARGET_RADIUS);
            newTarget.y = offsetY + (rand() % (GAME_AREA_HEIGHT - 2 * TARGET_RADIUS) + TARGET_RADIUS);

            // Kiểm tra xem target mới có trùng với cái nào trước đó không
            for (const auto& target : targets) {
                if (isOverlapping(newTarget.x, newTarget.y, target.x, target.y)) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition); // Nếu trùng thì tạo lại vị trí mới

        targets.push_back(newTarget);
    }
}

// Kiểm tra bắn trúng target
void handleMouseClick(int mouseX, int mouseY) {
    shotsFired++;
    for (auto it = targets.begin(); it != targets.end(); ++it) {
        int dx = mouseX - it->x;
        int dy = mouseY - it->y;
        if (dx * dx + dy * dy <= TARGET_RADIUS * TARGET_RADIUS) {
            shotsHit++;
            score += 1;
            Mix_PlayChannel(-1, hitSound, 0);
            targets.erase(it);
            spawnTarget();
            return;
        }
    }
}
void showResultScreen() {
    bool waiting = true;
    SDL_Event e;

    // Tạo nút "OK"
    Button okButton = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 3 + 200, 100, 50};

    std::string accuracy = (shotsFired > 0) ? std::to_string((shotsHit * 100) / shotsFired) + "%" : "100%";

    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                waiting = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                if (okButton.isInside(mouseX, mouseY)) {
                    waiting = false; // Nhấp vào "OK" để thoát
                }
            }
        }

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Hiển thị kết quả
        SDL_Color white = {255, 255, 255};
        renderText("Game Over", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 3, white);
        renderText("Score: " + std::to_string(score), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 3 + 50, white);
        renderText("Accuracy: " + accuracy, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 3 + 100, white);

        // Vẽ nút "OK"
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect okRect = {okButton.x, okButton.y, okButton.w, okButton.h};
        SDL_RenderFillRect(renderer, &okRect);
        
        // Vẽ viền nút "OK"
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &okRect);
        
        // Hiển thị chữ "OK" trên nút
        renderText("OK", okButton.x + 30, okButton.y + 15, white);

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }
}
void renderBackground(SDL_Renderer* renderer, SDL_Texture* backgroundTexture) {
    if (!backgroundTexture) return; // Tránh lỗi nếu texture bị NULL

    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL); // Phủ toàn màn hình
}


// Game Loop
void gameLoop() {
    bool running = true;
    SDL_Event e;
    startTime = SDL_GetTicks();

    // Spawn các target ban đầu
    for (int i = 0; i < MAX_TARGETS; i++) {
        spawnTarget();
    }

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                handleMouseClick(e.button.x, e.button.y);
            }
        }

        // Tính thời gian còn lại
        timeLeft = GAME_DURATION - (SDL_GetTicks() - startTime) / 1000;
        if (timeLeft <= 0) {
            running = false;
        }

        // Xóa màn hình mỗi frame
        SDL_RenderClear(renderer);

        // Vẽ nền (ảnh nền game)
        renderBackground(renderer, background);

        // Vẽ vùng chơi (có thể dùng alpha nếu muốn mờ)
        SDL_Rect gameArea = { offsetX, offsetY, GAME_AREA_WIDTH, GAME_AREA_HEIGHT };

        // Vẽ target bằng hình ảnh
        for (const auto& target : targets) {
            renderTarget(target.x, target.y);
        }

        // Vẽ text HUD
        SDL_Color white = { 255, 255, 255 };
        renderText("Score: " + std::to_string(score), 10, 10, white);
        std::string accuracy = (shotsFired > 0) ? std::to_string((shotsHit * 100) / shotsFired) + "%" : "100%";
        renderText("Accuracy: " + accuracy, 10, 40, white);
        renderText("Time: " + std::to_string(timeLeft) + "s", 10, 70, white);

        // Hiển thị frame lên màn hình
        SDL_RenderPresent(renderer);
    }
}


// Giải phóng tài nguyên
void close() {
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(targetTexture);
    Mix_FreeChunk(hitSound);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

// Main
int main(int argc, char* argv[]) {
    if (!init()) return -1;
    gameLoop();
    showResultScreen();
    close();
    return 0;
}
