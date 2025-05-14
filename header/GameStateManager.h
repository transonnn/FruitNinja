#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "Constants.h"
#include "Fruit.h"
#include "Slash.h"
#include "Button.h"
#include "TextureManager.h"
#include "AudioManager.h"

enum GameState {
    MENU,
    TRANSITION,
    PLAYING,
    SETTING,
    GAME_OVER
};

class GameStateManager {
public:
    GameStateManager();
    void initialize(SDL_Renderer* renderer, TextureManager& textureManager, AudioManager& audioManager, TTF_Font* font, SDL_Surface* newGameSurface, SDL_Surface* gameOverSurface);
    void update(float deltaTime);
    GameState getState() const { return gameState; }
    std::vector<Fruit>& getFruits() { return fruits; }
    std::vector<MissEffect>& getMissEffects() { return missEffects; }
    Slash& getSlash() { return currentSlash; }
    Button& getNewGameButton() { return newGameImage; }
    Button& getGameOverButton() { return gameOverImage; }
    bool isShowMenuElements() const { return showMenuElements; }
    Uint32 getTransitionStartTime() const { return transitionStartTime; }
    int getScore() const { return score; }
    int getLives() const { return lives; }
private:
    GameState gameState;
    std::vector<Fruit> fruits;
    std::vector<MissEffect> missEffects;
    Slash currentSlash;
    int score;
    int lives;
    Uint32 lastSpawn;
    Uint32 gameOverStartTime;
    float mouseX, mouseY;
    bool showMenuElements;
    bool transitioning;
    Uint32 transitionStartTime;
    Button newGameImage;
    Button gameOverImage;
    int newGameOrigW, newGameOrigH;
    int gameOverOrigW, gameOverOrigH;
    std::vector<std::string> fruitTypes;
    SDL_Renderer* renderer;
    TextureManager* textureManager;
    AudioManager* audioManager;
    TTF_Font* font;

    void spawnMenuFruit();
    void updateMenuAndTransition(float deltaTime);
    void updatePlaying(float deltaTime);
    void updateGameOver(float deltaTime);
};

#endif // GAME_STATE_MANAGER_H