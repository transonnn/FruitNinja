#include "RenderManager.h"
#include <string>

RenderManager::RenderManager()
    : renderer(nullptr), textureManager(nullptr), font(nullptr), bgTexture(nullptr),
      gameTitleTexture(nullptr), scoreImageTexture(nullptr), gameTitleOrigW(0), gameTitleOrigH(0) {
    for (int i = 0; i < 4; ++i) gameLifeTextures[i] = nullptr;
}

void RenderManager::initialize(SDL_Renderer* renderer_, TextureManager& textureManager_, TTF_Font* font_,
                               SDL_Texture* bgTexture_, SDL_Texture* gameTitleTexture_,
                               SDL_Texture* scoreImageTexture_, SDL_Texture* gameLifeTextures_[4],
                               int gameTitleOrigW_, int gameTitleOrigH_) {
    renderer = renderer_;
    textureManager = &textureManager_;
    font = font_;
    bgTexture = bgTexture_;
    gameTitleTexture = gameTitleTexture_;
    scoreImageTexture = scoreImageTexture_;
    for (int i = 0; i < 4; ++i) gameLifeTextures[i] = gameLifeTextures_[i];
    gameTitleOrigW = gameTitleOrigW_;
    gameTitleOrigH = gameTitleOrigH_;
}

void RenderManager::render(GameState state, const std::vector<Fruit>& fruits,
                           const std::vector<MissEffect>& missEffects, const Slash& slash,
                           const Button& newGameButton, const Button& gameOverButton,
                           bool showMenuElements, Uint32 transitionStartTime, int score, int lives) {
    SDL_Texture* missTexture = textureManager->loadMissTexture();
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
    if (state == MENU || state == TRANSITION) {
        renderMenuAndTransition(fruits, slash, newGameButton, showMenuElements, transitionStartTime);
    } else if (state == PLAYING) {
        renderPlaying(fruits, missEffects, slash, score, lives);
    } else if (state == GAME_OVER) {
        renderGameOver(slash, gameOverButton, score, lives);
    }
    if (state == TRANSITION) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsed = currentTime - transitionStartTime;
        Uint8 alpha = static_cast<Uint8>(255 * (elapsed / (float)TRANSITION_DURATION));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect overlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlayRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    SDL_DestroyTexture(missTexture);
    SDL_RenderPresent(renderer);
}

void RenderManager::renderMenuAndTransition(const std::vector<Fruit>& fruits, const Slash& slash,
                                            const Button& newGameButton, bool showMenuElements,
                                            Uint32 transitionStartTime) {
    Uint32 currentTime = SDL_GetTicks();
    if (showMenuElements) {
        float titleAspectRatio = static_cast<float>(gameTitleOrigW) / gameTitleOrigH;
        int titleWidth = SCREEN_WIDTH;
        int titleHeight = static_cast<int>(titleWidth / titleAspectRatio);
        SDL_Rect titleRect = {0, 0, titleWidth, titleHeight};
        SDL_RenderCopyEx(renderer, gameTitleTexture, nullptr, &titleRect, 0.0, nullptr, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, newGameButton.texture, nullptr, &newGameButton.rect,
                         newGameButton.angle, &newGameButton.center, SDL_FLIP_NONE);
    }
    for (const auto& fruit : fruits) {
        if (!fruit.active) continue;
        if (!fruit.sliced) {
            SDL_Rect fruitRect = {static_cast<int>(fruit.x - FRUIT_SIZE / 2), static_cast<int>(fruit.y - FRUIT_SIZE / 2), FRUIT_SIZE, FRUIT_SIZE};
            SDL_Point center = {FRUIT_SIZE / 2, FRUIT_SIZE / 2};
            SDL_RenderCopyEx(renderer, fruit.texture, nullptr, &fruitRect, fruit.rotationAngle, &center, SDL_FLIP_NONE);
        } else {
            SDL_Rect leftRect = {static_cast<int>(fruit.leftX - FRUIT_SIZE / 2), static_cast<int>(fruit.leftY - FRUIT_SIZE / 2), FRUIT_SIZE, FRUIT_SIZE};
            SDL_Point leftCenter = {FRUIT_SIZE / 2, FRUIT_SIZE / 2};
            SDL_RenderCopyEx(renderer, fruit.LeftslicedTexture, nullptr, &leftRect, fruit.angleLeft, &leftCenter, SDL_FLIP_NONE);
            SDL_Rect rightRect = {static_cast<int>(fruit.rightX - FRUIT_SIZE / 2), static_cast<int>(fruit.rightY - FRUIT_SIZE / 2), FRUIT_SIZE, FRUIT_SIZE};
            SDL_Point rightCenter = {FRUIT_SIZE / 2, FRUIT_SIZE / 2};
            SDL_RenderCopyEx(renderer, fruit.RightslicedTexture, nullptr, &rightRect, fruit.angleRight, &rightCenter, SDL_FLIP_NONE);
            if (fruit.splashActive) {
                Uint32 elapsed = currentTime - fruit.splashStartTime;
                Uint8 alpha = 255;
                float scale = 1.0f;
                if (elapsed < SPLASH_SCALE_DURATION) {
                    scale = 1.0f + 1.0f * (elapsed / (float)SPLASH_SCALE_DURATION);
                    alpha = 255;
                } else {
                    scale = 2.0f;
                    alpha = static_cast<Uint8>(255 * (1.0f - (elapsed - SPLASH_SCALE_DURATION) / (float)(SPLASH_FADE_DURATION - SPLASH_SCALE_DURATION)));
                }
                SDL_SetTextureAlphaMod(fruit.splashTexture, alpha);
                int splashSize = static_cast<int>(FRUIT_SIZE * scale);
                SDL_Rect splashRect = {
                    static_cast<int>(fruit.splashX - splashSize / 2),
                    static_cast<int>(fruit.splashY - splashSize / 2),
                    splashSize,
                    splashSize
                };
                SDL_RenderCopy(renderer, fruit.splashTexture, nullptr, &splashRect);
                SDL_SetTextureAlphaMod(fruit.splashTexture, 255);
            }
        }
    }
    if (!slash.points.empty()) {
        drawContinuousWhiteLine(renderer, slash.points);
    }
}

void RenderManager::renderPlaying(const std::vector<Fruit>& fruits, const std::vector<MissEffect>& missEffects,
                                 const Slash& slash, int score, int lives) {
    Uint32 currentTime = SDL_GetTicks();
    for (const auto& fruit : fruits) {
        if (!fruit.active || !fruit.splashActive) continue;
        Uint32 elapsed = currentTime - fruit.splashStartTime;
        Uint8 alpha = 255;
        float scale = 1.0f;
        if (elapsed < SPLASH_SCALE_DURATION) {
            scale = 1.0f + 1.0f * (elapsed / (float)SPLASH_SCALE_DURATION);
            alpha = 255;
        } else {
            scale = 2.0f;
            alpha = static_cast<Uint8>(255 * (1.0f - (elapsed - SPLASH_SCALE_DURATION) / (float)(SPLASH_FADE_DURATION - SPLASH_SCALE_DURATION)));
        }
        SDL_SetTextureAlphaMod(fruit.splashTexture, alpha);
        int splashSize = static_cast<int>(FRUIT_SIZE * scale);
        SDL_Rect splashRect = {
            static_cast<int>(fruit.splashX - splashSize / 2),
            static_cast<int>(fruit.splashY - splashSize / 2),
            splashSize,
            splashSize
        };
        SDL_RenderCopy(renderer, fruit.splashTexture, nullptr, &splashRect);
        SDL_SetTextureAlphaMod(fruit.splashTexture, 255);
    }
    SDL_Texture* missTexture = textureManager->loadMissTexture();
    for (const auto& miss : missEffects) {
        if (!miss.active) continue;
        Uint32 elapsed = currentTime - miss.startTime;
        Uint8 alpha = 255;
        if (elapsed < MISS_FADE_DURATION) {
            alpha = static_cast<Uint8>(255 * (1.0f - elapsed / (float)MISS_FADE_DURATION));
        } else {
            alpha = 0;
        }
        SDL_SetTextureAlphaMod(missTexture, alpha);
        SDL_Rect missRect = {static_cast<int>(miss.x - 12), SCREEN_HEIGHT - 25, 25, 25};
        SDL_RenderCopy(renderer, missTexture, nullptr, &missRect);
        SDL_SetTextureAlphaMod(missTexture, 255);
    }
    for (const auto& fruit : fruits) {
        if (!fruit.active) continue;
        if (!fruit.sliced) {
            SDL_Rect fruitRect = {static_cast<int>(fruit.x - FRUIT_SIZE / 2), static_cast<int>(fruit.y - FRUIT_SIZE / 2), FRUIT_SIZE, FRUIT_SIZE};
            SDL_Point center = {FRUIT_SIZE / 2, FRUIT_SIZE / 2};
            SDL_RenderCopyEx(renderer, fruit.texture, nullptr, &fruitRect, fruit.rotationAngle, &center, SDL_FLIP_NONE);
        } else if (fruit.type != "bomb") {
            SDL_Rect leftRect = {static_cast<int>(fruit.leftX - FRUIT_SIZE / 2), static_cast<int>(fruit.leftY - FRUIT_SIZE / 2), FRUIT_SIZE, FRUIT_SIZE};
            SDL_Point leftCenter = {FRUIT_SIZE / 2, FRUIT_SIZE / 2};
            SDL_RenderCopyEx(renderer, fruit.LeftslicedTexture, nullptr, &leftRect, fruit.angleLeft, &leftCenter, SDL_FLIP_NONE);
            SDL_Rect rightRect = {static_cast<int>(fruit.rightX - FRUIT_SIZE / 2), static_cast<int>(fruit.rightY - FRUIT_SIZE / 2), FRUIT_SIZE, FRUIT_SIZE};
            SDL_Point rightCenter = {FRUIT_SIZE / 2, FRUIT_SIZE / 2};
            SDL_RenderCopyEx(renderer, fruit.RightslicedTexture, nullptr, &rightRect, fruit.angleRight, &rightCenter, SDL_FLIP_NONE);
        }
    }
    if (!slash.points.empty()) {
        drawContinuousWhiteLine(renderer, slash.points);
    }
    SDL_Color white = {255, 255, 255, 255};
    int offsetX = 10;
    int scoreImgW, scoreImgH;
    SDL_QueryTexture(scoreImageTexture, nullptr, nullptr, &scoreImgW, &scoreImgH);
    SDL_Rect scoreImgRect = {offsetX, 10, scoreImgW, scoreImgH};
    SDL_RenderCopy(renderer, scoreImageTexture, nullptr, &scoreImgRect);
    offsetX += scoreImgW + 5;
    SDL_Texture* colonTexture = textureManager->createTextTexture(font, ":", white);
    if (colonTexture) {
        int colonW, colonH;
        SDL_QueryTexture(colonTexture, nullptr, nullptr, &colonW, &colonH);
        SDL_Rect colonRect = {offsetX, 10, colonW, colonH};
        SDL_RenderCopy(renderer, colonTexture, nullptr, &colonRect);
        SDL_DestroyTexture(colonTexture);
        offsetX += colonW + 5;
    }
    std::string scoreText = std::to_string(score);
    SDL_Texture* scoreNumTexture = textureManager->createTextTexture(font, scoreText, white);
    if (scoreNumTexture) {
        int scoreNumW, scoreNumH;
        SDL_QueryTexture(scoreNumTexture, nullptr, nullptr, &scoreNumW, &scoreNumH);
        SDL_Rect scoreNumRect = {offsetX, 10, scoreNumW, scoreNumH};
        SDL_RenderCopy(renderer, scoreNumTexture, nullptr, &scoreNumRect);
        SDL_DestroyTexture(scoreNumTexture);
    }
    if (lives >= 0 && lives <= 3) {
        int lw, lh;
        SDL_QueryTexture(gameLifeTextures[lives], nullptr, nullptr, &lw, &lh);
        SDL_Rect lifeRect = {SCREEN_WIDTH - lw - 10, 10, lw, lh};
        SDL_RenderCopy(renderer, gameLifeTextures[lives], nullptr, &lifeRect);
    }
}

void RenderManager::renderGameOver(const Slash& slash, const Button& gameOverButton, int score, int lives) {
    SDL_RenderCopyEx(renderer, gameOverButton.texture, nullptr, &gameOverButton.rect,
                     gameOverButton.angle, &gameOverButton.center, SDL_FLIP_NONE);
    if (!slash.points.empty()) {
        drawContinuousWhiteLine(renderer, slash.points);
    }
    SDL_Color white = {255, 255, 255, 255};
    int offsetX = 10;
    int scoreImgW, scoreImgH;
    SDL_QueryTexture(scoreImageTexture, nullptr, nullptr, &scoreImgW, &scoreImgH);
    SDL_Rect scoreImgRect = {offsetX, 10, scoreImgW, scoreImgH};
    SDL_RenderCopy(renderer, scoreImageTexture, nullptr, &scoreImgRect);
    offsetX += scoreImgW + 5;
    SDL_Texture* colonTexture = textureManager->createTextTexture(font, ":", white);
    if (colonTexture) {
        int colonW, colonH;
        SDL_QueryTexture(colonTexture, nullptr, nullptr, &colonW, &colonH);
        SDL_Rect colonRect = {offsetX, 10, colonW, colonH};
        SDL_RenderCopy(renderer, colonTexture, nullptr, &colonRect);
        SDL_DestroyTexture(colonTexture);
        offsetX += colonW + 5;
    }
    std::string scoreText = std::to_string(score);
    SDL_Texture* scoreNumTexture = textureManager->createTextTexture(font, scoreText, white);
    if (scoreNumTexture) {
        int scoreNumW, scoreNumH;
        SDL_QueryTexture(scoreNumTexture, nullptr, nullptr, &scoreNumW, &scoreNumH);
        SDL_Rect scoreNumRect = {offsetX, 10, scoreNumW, scoreNumH};
        SDL_RenderCopy(renderer, scoreNumTexture, nullptr, &scoreNumRect);
        SDL_DestroyTexture(scoreNumTexture);
    }
    if (lives >= 0 && lives <= 3) {
        int lw, lh;
        SDL_QueryTexture(gameLifeTextures[lives], nullptr, nullptr, &lw, &lh);
        SDL_Rect lifeRect = {SCREEN_WIDTH - lw - 10, 10, lw, lh};
        SDL_RenderCopy(renderer, gameLifeTextures[lives], nullptr, &lifeRect);
    }
}