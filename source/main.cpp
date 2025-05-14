#include <iostream>
#include "Game.h"

int main(int argc, char* args[]) {
    Game game;
    if (!game.initialize()) {
        std::cerr << "Game initialization failed" << std::endl;
        return 1;
    }
    game.run();
    return 0;
}