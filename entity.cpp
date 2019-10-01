#include "render.hpp"

class Model;

Entity::Entity(const Model & model) { this->model = model; }

void Entity::render() { this->model.render(); }

void Entity::cleanUp() { this->model.cleanUp(); }
