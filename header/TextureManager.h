#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextureManager {
public:
    TextureManager(SDL_Renderer* renderer);
    SDL_Texture* loadFruitTexture(const std::string& type, const std::string& state);
    SDL_Texture* loadMissTexture();
    SDL_Texture* loadScoreTexture();
    SDL_Texture* createTextTexture(TTF_Font* font, const std::string& text, SDL_Color color);
private:
    SDL_Renderer* renderer;
};

#endif // TEXTURE_MANAGER_H