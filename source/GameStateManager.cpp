#include "GameStateManager.h"
#include "Utils.h"
#include <algorithm>
#include <iostream>

GameStateManager::GameStateManager()
    : gameState(MENU), score(0), lives(3), lastSpawn(0), gameOverStartTime(0),
      mouseX(0.0f), mouseY(0.0f), showMenuElements(true), transitioning(false),
      transitionStartTime(0), renderer(nullptr), textureManager(nullptr),
      audioManager(nullptr), font(nullptr), newGameOrigW(0), newGameOrigH(0),
      gameOverOrigW(0), gameOverOrigH(0) {
    fruitTypes = {"apple", "basaha", "peach", "sandia"};
}

void GameStateManager::initialize(SDL_Renderer* renderer_, TextureManager& textureManager_,
                                  AudioManager& audioManager_, TTF_Font* font_,
                                  SDL_Surface* newGameSurface, SDL_Surface* gameOverSurface) {
    renderer = renderer_;
    textureManager = &textureManager_;
    audioManager = &audioManager_;
    font = font_;
    newGameImage.texture = SDL_CreateTextureFromSurface(renderer, newGameSurface);
    newGameOrigW = newGameSurface->w;
    newGameOrigH = newGameSurface->h;
    gameOverImage.texture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    gameOverOrigW = gameOverSurface->w;
    gameOverOrigH = gameOverSurface->h;
    newGameImage.rect.w = static_cast<int>(newGameOrigW * 1.1);
    newGameImage.rect.h = static_cast<int>(newGameOrigH * 1.1);
    newGameImage.rect.x = static_cast<int>(SCREEN_WIDTH * 0.75 - newGameImage.rect.w / 2);
    newGameImage.rect.y = static_cast<int>(SCREEN_HEIGHT * 0.75 - newGameImage.rect.h / 2);
    newGameImage.center = {newGameImage.rect.w / 2, newGameImage.rect.h / 2};
    newGameImage.angle = 0.0;
    float scale = std::min(0.5f, std::min(SCREEN_WIDTH * 0.5f / gameOverOrigW, SCREEN_HEIGHT * 0.5f / gameOverOrigH));
    gameOverImage.rect.w = static_cast<int>(gameOverOrigW * scale);
    gameOverImage.rect.h = static_cast<int>(gameOverOrigH * scale);
    gameOverImage.rect.x = (SCREEN_WIDTH - gameOverImage.rect.w) / 2;
    gameOverImage.rect.y = (SCREEN_HEIGHT - gameOverImage.rect.h) / 2;
    gameOverImage.center = {gameOverImage.rect.w / 2, newGameImage.rect.h / 2};
    spawnMenuFruit();
}

void GameStateManager::update(float deltaTime) {
    Uint32 currentTime = SDL_GetTicks();
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    mouseX = static_cast<float>(mx);
    mouseY = static_cast<float>(my);
    if (currentSlash.active && currentSlash.points.size() >= 2) {
        updateSlashTail(currentSlash, deltaTime, mouseX, mouseY, currentTime);
        trimSlashToMaxLength(currentSlash);
    }
    if (gameState == MENU || gameState == TRANSITION) {
        updateMenuAndTransition(deltaTime);
    } else if (gameState == PLAYING) {
        updatePlaying(deltaTime);
    } else if (gameState == GAME_OVER) {
        updateGameOver(deltaTime);
    }
}

void GameStateManager::updateMenuAndTransition(float deltaTime) {
    Uint32 currentTime = SDL_GetTicks();
    newGameImage.angle += 90.0 * deltaTime;
    if (newGameImage.angle >= 360.0) newGameImage.angle -= 360.0;
    for (auto& fruit : fruits) {
        if (!fruit.sliced) {
            fruit.rotationAngle -= 270.0 * deltaTime;
            if (fruit.rotationAngle <= -360.0) fruit.rotationAngle += 360.0;
        } else {
            fruit.leftX += fruit.leftVx * deltaTime;
            fruit.leftY += fruit.leftVy * deltaTime;
            fruit.leftVy += FRUIT_GRAVITY * deltaTime;
            fruit.rightX += fruit.rightVx * deltaTime;
            fruit.rightY += fruit.rightVy * deltaTime;
            fruit.rightVy += FRUIT_GRAVITY * deltaTime;
            fruit.angleLeft += 270.0 * deltaTime;
            fruit.angleRight -= 270.0 * deltaTime;
            if (fruit.angleLeft >= 360.0) fruit.angleLeft -= 360.0;
            if (fruit.angleRight <= -360.0) fruit.angleRight += 360.0;
            if (fruit.splashActive) {
                Uint32 elapsed = currentTime - fruit.splashStartTime;
                if (elapsed >= SPLASH_FADE_DURATION) fruit.splashActive = false;
            }
            if (fruit.leftY > SCREEN_HEIGHT + 100 && fruit.rightY > SCREEN_HEIGHT + 100) {
                fruit.active = false;
            }
        }
    }
    fruits.erase(std::remove_if(fruits.begin(), fruits.end(),
        [](const Fruit& f) { return !f.active; }), fruits.end());
    if (gameState == TRANSITION) {
        if (currentTime - transitionStartTime >= TRANSITION_DURATION) {
            gameState = PLAYING;
            transitioning = false;
            score = 0;
            lives = 3;
            fruits.clear();
            missEffects.clear();
            currentSlash = {{}, false, 0, 0, 0, 0};
        }
    }
    if (gameState == MENU && currentSlash.active && currentSlash.points.size() >= 2) {
        for (auto& fruit : fruits) {
            if (!fruit.active || fruit.sliced) continue;
            for (size_t i = 1; i < currentSlash.points.size(); ++i) {
                float x1 = currentSlash.points[i - 1].x;
                float y1 = currentSlash.points[i - 1].y;
                float x2 = currentSlash.points[i].x;
                float y2 = currentSlash.points[i].y;
                if (lineIntersectsCircle(x1, y1, x2, y2, fruit.x, fruit.y, HITBOX_RADIUS)) {
                    fruit.sliced = true;
                    audioManager->playSplatterSound();
                    fruit.splashX = fruit.x;
                    fruit.splashY = fruit.y;
                    fruit.splashStartTime = currentTime;
                    fruit.splashActive = true;
                    fruit.leftX = fruit.x;
                    fruit.leftY = fruit.y;
                    fruit.rightX = fruit.x;
                    fruit.rightY = fruit.y;
                    fruit.leftVx = -50.0f;
                    fruit.leftVy = -100.0f;
                    fruit.rightVx = 50.0f;
                    fruit.rightVy = -100.0f;
                    showMenuElements = false;
                    transitioning = true;
                    transitionStartTime = currentTime;
                    gameState = TRANSITION;
                    break;
                }
            }
            if (gameState == TRANSITION) break;
        }
    }
}

void GameStateManager::updatePlaying(float deltaTime) {
    Uint32 currentTime = SDL_GetTicks();
    const Uint32 spawnInterval = 1000;
    if (currentTime - lastSpawn >= spawnInterval && fruits.empty()) {
        int minSpawn = 2;
        int maxSpawn = 4;
        if (score >= 15) {
            int increment = (score - 15) / 10;
            minSpawn += increment;
            maxSpawn += increment;
        }
        int numToSpawn = rand() % (maxSpawn - minSpawn + 1) + minSpawn;
        numToSpawn = std::min(numToSpawn, static_cast<int>((SCREEN_WIDTH - 150) / MIN_SPAWN_SEPARATION));
        audioManager->playThrowFruitSound();
        std::vector<float> usedXPositions;
        for (int i = 0; i < numToSpawn; ++i) {
            Fruit fruit;
            if (rand() % 100 < 20) {
                fruit.type = "bomb";
            } else {
                fruit.type = fruitTypes[rand() % fruitTypes.size()];
            }
            bool validPosition = false;
            int attempts = 0;
            const int maxAttempts = 10;
            float newX;
            while (attempts < maxAttempts && !validPosition) {
                newX = rand() % (SCREEN_WIDTH - 150) + 75;
                validPosition = true;
                for (float usedX : usedXPositions) {
                    if (std::abs(newX - usedX) < MIN_SPAWN_SEPARATION) {
                        validPosition = false;
                        break;
                    }
                }
                attempts++;
            }
            if (!validPosition) continue;
            fruit.x = newX;
            usedXPositions.push_back(newX);
            fruit.y = SCREEN_HEIGHT;
            fruit.vx = 0.0f;
            fruit.vy = -((rand() % 100 + 200) * 3 * 1.5 * 0.75);
            fruit.active = true;
            fruit.sliced = false;
            fruit.rotationAngle = 0.0;
            fruit.texture = textureManager->loadFruitTexture(fruit.type, "whole");
            if (fruit.type != "bomb") {
                fruit.LeftslicedTexture = textureManager->loadFruitTexture(fruit.type, "left");
                fruit.RightslicedTexture = textureManager->loadFruitTexture(fruit.type, "right");
                fruit.splashTexture = textureManager->loadFruitTexture(fruit.type, "splash");
            } else {
                fruit.LeftslicedTexture = nullptr;
                fruit.RightslicedTexture = nullptr;
                fruit.splashTexture = nullptr;
            }
            fruit.splashX = 0.0f;
            fruit.splashY = 0.0f;
            fruit.splashStartTime = 0;
            fruit.splashActive = false;
            fruit.leftX = 0.0f;
            fruit.leftY = 0.0f;
            fruit.rightX = 0.0f;
            fruit.rightY = 0.0f;
            fruit.leftVx = 0.0f;
            fruit.leftVy = 0.0f;
            fruit.rightVx = 0.0f;
            fruit.rightVy = 0.0f;
            fruit.angleLeft = 0.0;
            fruit.angleRight = 0.0;
            if (fruit.texture) {
                fruits.push_back(fruit);
            }
        }
        lastSpawn = currentTime;
    }
    for (auto& fruit : fruits) {
        if (!fruit.active) continue;
        if (!fruit.sliced) {
            fruit.x += fruit.vx * deltaTime;
            fruit.y += fruit.vy * deltaTime;
            if (fruit.type == "bomb") {
                fruit.vy += BOMB_GRAVITY * deltaTime;
            } else {
                fruit.vy += FRUIT_GRAVITY * deltaTime;
            }
            fruit.rotationAngle += 270.0 * deltaTime;
            if (fruit.rotationAngle >= 360.0) fruit.rotationAngle -= 360.0;
            if (fruit.y < 0) {
                fruit.y = 0;
                fruit.vy = -fruit.vy * 0.5;
            }
            if (fruit.y > SCREEN_HEIGHT + 100 && !fruit.sliced) {
                if (fruit.type != "bomb") {
                    MissEffect miss;
                    miss.x = fruit.x;
                    miss.startTime = currentTime;
                    miss.active = true;
                    missEffects.push_back(miss);
                    lives--;
                    if (lives < 0) lives = 0;
                    if (lives == 0) {
                        gameState = GAME_OVER;
                        gameOverStartTime = currentTime;
                    }
                }
                fruit.active = false;
            }
        } else if (fruit.type != "bomb") {
            fruit.leftX += fruit.leftVx * deltaTime;
            fruit.leftY += fruit.leftVy * deltaTime;
            fruit.leftVy += FRUIT_GRAVITY * deltaTime;
            fruit.rightX += fruit.rightVx * deltaTime;
            fruit.rightY += fruit.rightVy * deltaTime;
            fruit.rightVy += FRUIT_GRAVITY * deltaTime;
            fruit.angleLeft += 270.0 * deltaTime;
            fruit.angleRight -= 270.0 * deltaTime;
            if (fruit.angleLeft >= 360.0) fruit.angleLeft -= 360.0;
            if (fruit.angleRight <= -360.0) fruit.angleRight += 360.0;
            if (fruit.splashActive) {
                Uint32 elapsed = currentTime - fruit.splashStartTime;
                if (elapsed >= SPLASH_FADE_DURATION) fruit.splashActive = false;
            }
            if (fruit.leftY > SCREEN_HEIGHT + 100 && fruit.rightY > SCREEN_HEIGHT + 100) {
                fruit.active = false;
            }
        }
    }
    for (auto& miss : missEffects) {
        if (!miss.active) continue;
        Uint32 elapsed = currentTime - miss.startTime;
        if (elapsed >= MISS_FADE_DURATION) miss.active = false;
    }
    if (gameState == PLAYING && currentSlash.active && currentSlash.points.size() >= 2) {
        for (auto& fruit : fruits) {
            if (!fruit.active || fruit.sliced) continue;
            for (size_t i = 1; i < currentSlash.points.size(); ++i) {
                float x1 = currentSlash.points[i - 1].x;
                float y1 = currentSlash.points[i - 1].y;
                float x2 = currentSlash.points[i].x;
                float y2 = currentSlash.points[i].y;
                if (lineIntersectsCircle(x1, y1, x2, y2, fruit.x, fruit.y, HITBOX_RADIUS)) {
                    fruit.sliced = true;
                    if (fruit.type == "bomb") {
                        audioManager->playExplodeSound();
                        gameState = GAME_OVER;
                        gameOverStartTime = currentTime;
                        break;
                    } else {
                        audioManager->playSplatterSound();
                        score += 1;
                        fruit.splashX = fruit.x;
                        fruit.splashY = fruit.y;
                        fruit.splashStartTime = currentTime;
                        fruit.splashActive = true;
                        fruit.leftX = fruit.x;
                        fruit.leftY = fruit.y;
                        fruit.rightX = fruit.x;
                        fruit.rightY = fruit.y;
                        fruit.leftVx = fruit.vx - 50.0f;
                        fruit.leftVy = -100.0f;
                        fruit.rightVx = fruit.vx + 50.0f;
                        fruit.rightVy = -100.0f;
                    }
                    break;
                }
            }
        }
    }
    fruits.erase(std::remove_if(fruits.begin(), fruits.end(),
        [](const Fruit& f) { return !f.active; }), fruits.end());
    missEffects.erase(std::remove_if(missEffects.begin(), missEffects.end(),
        [](const MissEffect& m) { return !m.active; }), missEffects.end());
}

void GameStateManager::updateGameOver(float deltaTime) {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - gameOverStartTime >= GAME_OVER_DURATION) {
        gameState = MENU;
        score = 0;
        lives = 3;
        fruits.clear();
        missEffects.clear();
        currentSlash = {{}, false, 0, 0, 0, 0};
        showMenuElements = true;
        transitioning = false;
        spawnMenuFruit();
    }
}

void GameStateManager::spawnMenuFruit() {
    fruits.clear();
    Fruit startFruit;
    startFruit.type = fruitTypes[rand() % fruitTypes.size()];
    int circleX = newGameImage.rect.x + newGameImage.rect.w / 2;
    int circleY = newGameImage.rect.y + newGameImage.rect.h / 2;
    int circleRadius = std::min(newGameImage.rect.w, newGameImage.rect.h) / 4 - FRUIT_SIZE / 2;
    float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
    float radius = static_cast<float>(rand()) / RAND_MAX * circleRadius;
    startFruit.x = circleX + radius * cos(angle);
    startFruit.y = circleY + radius * sin(angle);
    startFruit.vx = 0.0f;
    startFruit.vy = 0.0f;
    startFruit.active = true;
    startFruit.sliced = false;
    startFruit.rotationAngle = 0.0;
    startFruit.texture = textureManager->loadFruitTexture(startFruit.type, "whole");
    startFruit.LeftslicedTexture = textureManager->loadFruitTexture(startFruit.type, "left");
    startFruit.RightslicedTexture = textureManager->loadFruitTexture(startFruit.type, "right");
    startFruit.splashTexture = textureManager->loadFruitTexture(startFruit.type, "splash");
    startFruit.splashX = 0.0f;
    startFruit.splashY = 0.0f;
    startFruit.splashStartTime = 0;
    startFruit.splashActive = false;
    startFruit.leftX = 0.0f;
    startFruit.leftY = 0.0f;
    startFruit.rightX = 0.0f;
    startFruit.rightY = 0.0f;
    startFruit.leftVx = 0.0f;
    startFruit.leftVy = 0.0f;
    startFruit.rightVx = 0.0f;
    startFruit.rightVy = 0.0f;
    startFruit.angleLeft = 0.0;
    startFruit.angleRight = 0.0;
    if (startFruit.texture && startFruit.LeftslicedTexture && startFruit.RightslicedTexture && startFruit.splashTexture) {
        fruits.push_back(startFruit);
    } else {
        std::cerr << "Failed to load all textures for fruit type: " << startFruit.type << std::endl;
    }
}