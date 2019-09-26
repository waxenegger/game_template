#ifndef GAME_HPP
#define GAME_HPP

    #include <iostream>
    #include <ctype.h>

    #include <SDL.h>

    #include <glm/glm.hpp>

    #include <GL/glew.h>

    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>


    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    class Game {
    private:
        SDL_Window * window = nullptr;
        SDL_GLContext glContext = nullptr;
        void render();
        void cleanUp();

    public:
        bool init();
        void run();
        virtual ~Game();
    };

#endif
