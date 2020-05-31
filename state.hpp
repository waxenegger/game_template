#ifndef STATE_HPP_
#define STATE_HPP_

#include "render.hpp"

class RenderableGroup {
    private:
        std::string id = "";
        std::vector<Renderable*> content;
    public:
        RenderableGroup(std::string id);
        ~RenderableGroup();
        void render();
        void addRenderable(Renderable * renderable);
};

class GameState {
    private:
        std::string root = "";
        std::map<std::string, RenderableGroup *> scene;
        Terrain * terrain = nullptr;

    public:
        GameState(std::string & root);
        void init();
        void render();
        void addRenderable(Renderable * renderable);
        ~GameState();
};

#endif /* STATE_HPP_ */
