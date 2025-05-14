#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() : throwFruitSound(nullptr), splatterSound(nullptr), explodeSound(nullptr) {}

AudioManager::~AudioManager() {
    if (throwFruitSound) Mix_FreeChunk(throwFruitSound);
    if (splatterSound) Mix_FreeChunk(splatterSound);
    if (explodeSound) Mix_FreeChunk(explodeSound);
}

bool AudioManager::initialize() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        return false;
    }
    throwFruitSound = Mix_LoadWAV("../resources/sound/throw-fruit.mp3");
    if (!throwFruitSound) {
        std::cerr << "Failed to load throw-fruit.mp3: " << Mix_GetError() << std::endl;
        return false;
    }
    splatterSound = Mix_LoadWAV("../resources/sound/splatter.mp3");
    if (!splatterSound) {
        std::cerr << "Failed to load splatter.mp3: " << Mix_GetError() << std::endl;
        Mix_FreeChunk(throwFruitSound);
        return false;
    }
    explodeSound = Mix_LoadWAV("../resources/sound/bomb-explode.mp3");
    if (!explodeSound) {
        std::cerr << "Failed to load bomb-explode.mp3: " << Mix_GetError() << std::endl;
        Mix_FreeChunk(throwFruitSound);
        Mix_FreeChunk(splatterSound);
        return false;
    }
    return true;
}

void AudioManager::playThrowFruitSound() {
    Mix_PlayChannel(-1, throwFruitSound, 0);
}

void AudioManager::playSplatterSound() {
    Mix_PlayChannel(-1, splatterSound, 0);
}

void AudioManager::playExplodeSound() {
    Mix_PlayChannel(-1, explodeSound, 0);
}