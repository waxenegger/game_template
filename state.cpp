#include "state.hpp"

GameState::GameState(std::string & root) {
    this->root = root;
}

void GameState::init() {
    this->terrain = new Terrain(this->root);
    this->terrain->init();
    this->sky = new SkyBox(this->root, "sky");
    this->sky->init();
}

void GameState::render() {
    if (this->terrain != nullptr) this->terrain->render();

    for (auto & sceneEntry : this->scene) sceneEntry.second->render();

    if (this->sky != nullptr) this->sky->render();
}

void GameState::addRenderable(Renderable * renderable) {
    if (renderable == nullptr) return;

    RenderableGroup * group = this->scene[renderable->getRenderableID()];
    if (group == nullptr) {
        group = new RenderableGroup(renderable->getRenderableID());
        this->scene[renderable->getRenderableID()] = group;
    }
    group->addRenderable(renderable);
}

GameState::~GameState() {
    if (this->terrain != nullptr) {
        this->terrain->cleanUp();
        if (this->terrain != nullptr) delete this->terrain;
    }

    for (auto & sceneEntry : this->scene) delete sceneEntry.second;

    if (this->sky != nullptr) {
        this->sky->cleanUp();
        if (this->sky != nullptr) delete this->sky;
    }

    this->scene.clear();
}

