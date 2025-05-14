#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
#include "Constants.h"
#include "Fruit.h"
#include "Slash.h"
#include "Button.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "GameStateManager.h"
#include "RenderManager.h"
#include "InputManager.h"

class Game {
public:
    Game();
    ~Game();
    bool initialize();
    void run();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    GameStateManager stateManager;
    RenderManager renderManager;
    InputManager inputManager;
    TextureManager textureManager;
    AudioManager audioManager;
    SDL_Texture* bgTexture;
    SDL_Texture* gameTitleTexture;
    SDL_Texture* gameLifeTextures[4];
    SDL_Texture* scoreImageTexture;
    int gameTitleOrigW, gameTitleOrigH;
    int newGameOrigW, newGameOrigH;
    int gameOverOrigW, gameOverOrigH;
};

#endif // GAME_H