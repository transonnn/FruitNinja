#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL_mixer.h>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    bool initialize();
    void playThrowFruitSound();
    void playSplatterSound();
    void playExplodeSound();
private:
    Mix_Chunk* throwFruitSound;
    Mix_Chunk* splatterSound;
    Mix_Chunk* explodeSound;
};

#endif // AUDIO_MANAGER_H