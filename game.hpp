#ifndef GAME_HPP
#define GAME_HPP

    #include "includes.hpp"
	#include "renderable.hpp"

    class Game {
    private:
		int width = 640;
		int height = 480;

		Uint32 frameStart = 0;
		Uint32 frameDuration = 0;

		bool wireframe = false;

        SDL_Window * window = nullptr;
        SDL_GLContext glContext = nullptr;

        std::vector<Renderable> scene;

        void clearScreen(float r, float g, float b, float a);
        void render();
        void cleanUp();

    public:
        bool init();
        void run();
        float getAspectRatio() const;
		void toggleWireframe();
        void resize(int width, int height);
        float getLastFrameDuration() const;
        virtual ~Game();
    };

#endif
