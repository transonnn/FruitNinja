#ifndef INPUT_MANAGER_H
   #define INPUT_MANAGER_H

   #include <SDL2/SDL.h>
   #include <vector>
   #include <cmath>
   #include "Slash.h"    // Bao gồm định nghĩa của SlashPoint và Slash
   #include "Button.h"   // Bao gồm định nghĩa của Button

   #define M_PI 3.14159265358979323846

   // Assuming these are defined elsewhere (e.g., in main.cpp or another header)
   extern int SCREEN_WIDTH;
   extern int SCREEN_HEIGHT;
   extern const float MAX_SLASH_LENGTH;

   class InputManager {
   public:
       InputManager();
       void initialize(SDL_Window* window, Slash& slash, Button& newGameButton, Button& gameOverButton);
       bool handleEvents();
       void updateWindowSize(int newWidth, int newHeight);

   private:
       SDL_Window* window;
       Slash* slash;
       Button* newGameButton;
       Button* gameOverButton;
       int newGameOrigW, newGameOrigH;
       int gameOverOrigW, gameOverOrigH;

       void trimSlashToMaxLength(Slash& slash);
   };

   #endif // INPUT_MANAGER_H