#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Constants.h"
#include "Fruit.h"
#include "Slash.h"
#include "Button.h"
#include "TextureManager.h"
#include "GameStateManager.h"

class RenderManager {
public:
    RenderManager();
    void initialize(SDL_Renderer* renderer, TextureManager& textureManager, TTF_Font* font,
                    SDL_Texture* bgTexture, SDL_Texture* gameTitleTexture, SDL_Texture* scoreImageTexture,
                    SDL_Texture* gameLifeTextures[4], int gameTitleOrigW, int gameTitleOrigH);
    void render(GameState state, const std::vector<Fruit>& fruits, const std::vector<MissEffect>& missEffects,
                const Slash& slash, const Button& newGameButton, const Button& gameOverButton,
                bool showMenuElements, Uint32 transitionStartTime, int score, int lives);
private:
    SDL_Renderer* renderer;
    TextureManager* textureManager;
    TTF_Font* font;
    SDL_Texture* bgTexture;
    SDL_Texture* gameTitleTexture;
    SDL_Texture* scoreImageTexture;
    SDL_Texture* gameLifeTextures[4];
    int gameTitleOrigW, gameTitleOrigH;

    void renderMenuAndTransition(const std::vector<Fruit>& fruits, const Slash& slash,
                                 const Button& newGameButton, bool showMenuElements, Uint32 transitionStartTime);
    void renderPlaying(const std::vector<Fruit>& fruits, const std::vector<MissEffect>& missEffects,
                       const Slash& slash, int score, int lives);
    void renderGameOver(const Slash& slash, const Button& gameOverButton, int score, int lives);
};

#endif // RENDER_MANAGER_H