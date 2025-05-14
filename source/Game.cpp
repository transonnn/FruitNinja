#include "Game.h"
#include <iostream>

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

Game::Game()
    : window(nullptr), renderer(nullptr), font(nullptr), textureManager(nullptr),
      bgTexture(nullptr), gameTitleTexture(nullptr), scoreImageTexture(nullptr),
      gameTitleOrigW(0), gameTitleOrigH(0), newGameOrigW(0), newGameOrigH(0),
      gameOverOrigW(0), gameOverOrigH(0) {
    for (int i = 0; i < 4; ++i) gameLifeTextures[i] = nullptr;
}

Game::~Game() {
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(gameTitleTexture);
    SDL_DestroyTexture(scoreImageTexture);
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(gameLifeTextures[i]);
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool Game::initialize() {
    srand(time(nullptr));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    if (!audioManager.initialize()) {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    window = SDL_CreateWindow("Fruit Ninja", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return false;
    }
    textureManager = TextureManager(renderer);
    font = TTF_OpenFont("../resources/font/arial.ttf", 36);
    if (!font) {
        std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }
    SDL_Surface* bgSurface = IMG_Load("../resources/assets/bg.jpg");
    if (!bgSurface) {
        std::cerr << "Failed to load background: " << IMG_GetError() << std::endl;
        return false;
    }
    bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    SDL_Surface* gameTitleSurface = IMG_Load("../resources/assets/gametitle.png");
    if (!gameTitleSurface) {
        std::cerr << "Failed to load game title: " << IMG_GetError() << std::endl;
        return false;
    }
    gameTitleTexture = SDL_CreateTextureFromSurface(renderer, gameTitleSurface);
    gameTitleOrigW = gameTitleSurface->w;
    gameTitleOrigH = gameTitleSurface->h;
    SDL_FreeSurface(gameTitleSurface);
    SDL_Surface* newGameSurface = IMG_Load("../resources/assets/newgame.png");
    if (!newGameSurface) {
        std::cerr << "Failed to load new game image: " << IMG_GetError() << std::endl;
        return false;
    }
    SDL_Surface* gameOverSurface = IMG_Load("../resources/assets/gameover.png");
    if (!gameOverSurface) {
        std::cerr << "Failed to load game over image: " << IMG_GetError() << std::endl;
        SDL_FreeSurface(newGameSurface);
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        std::string path = "../resources/assets/gamelife-" + std::to_string(i) + ".png";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << std::endl;
            SDL_FreeSurface(newGameSurface);
            SDL_FreeSurface(gameOverSurface);
            return false;
        }
        gameLifeTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    scoreImageTexture = textureManager.loadScoreTexture();
    if (!scoreImageTexture) {
        std::cerr << "Failed to load score texture" << std::endl;
        SDL_FreeSurface(newGameSurface);
        SDL_FreeSurface(gameOverSurface);
        return false;
    }
    // Initialize managers
    stateManager.initialize(renderer, textureManager, audioManager, font, newGameSurface, gameOverSurface);
    renderManager.initialize(renderer, textureManager, font, bgTexture, gameTitleTexture, scoreImageTexture, gameLifeTextures, gameTitleOrigW, gameTitleOrigH);
    inputManager.initialize(window, stateManager.getSlash(), stateManager.getNewGameButton(), stateManager.getGameOverButton());
    SDL_FreeSurface(newGameSurface);
    SDL_FreeSurface(gameOverSurface);
    return true;
}

void Game::run() {
    bool running = true;
    Uint32 lastFrameTime = SDL_GetTicks();
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;
        if (!inputManager.handleEvents()) {
            running = false;
        }
        stateManager.update(deltaTime);
        renderManager.render(stateManager.getState(), stateManager.getFruits(), stateManager.getMissEffects(), stateManager.getSlash(), stateManager.getNewGameButton(), stateManager.getGameOverButton(), stateManager.isShowMenuElements(), stateManager.getTransitionStartTime(), stateManager.getScore(), stateManager.getLives());
    }
}