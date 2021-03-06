#ifndef GAME_HPP
#define GAME_HPP

#include "render.hpp"
#include "world.hpp"
#include "state.hpp"

class Game {
    private:
        std::string root = "./";

        int width = DEFAULT_WIDTH;
        int height = DEFAULT_HEIGHT;

        bool wireframe = false;

        bool quit = false;

        GameState * state = nullptr;

        SDL_Window * window = nullptr;
        SDL_GLContext glContext = nullptr;

        ModelFactory * factory = nullptr;

        World * world = World::instance();
        Camera * camera = Camera::instance(-5.0f, 7.0f, -5.0f);

        void clearScreen(float r, float g, float b, float a);
        void cleanUp();

    public:
        Game(std::string root);
        std::string getRoot() { return this->root; }
        bool init();
        void run();
        float getAspectRatio() const;
        void resize(int width, int height);
        float getLastFrameDuration() const;
        virtual ~Game();
        void createTestModels();
        static std::map<std::string, std::shared_ptr<Texture>> TEXTURES;
};

#endif
