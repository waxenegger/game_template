#ifndef GAME_HPP
#define GAME_HPP

#include "render.hpp"

class Game {
    private:
        std::string root_path;
        int width = DEFAULT_WIDTH;
        int height = DEFAULT_HEIGHT;

        Uint32 frameStart = 0;
        Uint32 frameDuration = 0;

        bool wireframe = false;

        SDL_Window * window = nullptr;
        SDL_GLContext glContext = nullptr;

        std::vector<std::unique_ptr<Entity>> scene;

        void clearScreen(float r, float g, float b, float a);
        void render();
        void cleanUp();

    public:
        bool init(const std::string path = "");
        void run();
        float getAspectRatio() const;
        void toggleWireframe();
        void resize(int width, int height);
        float getLastFrameDuration() const;
        virtual ~Game();
        void createTestModels();
};

#endif
