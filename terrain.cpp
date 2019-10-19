#include "render.hpp"

Terrain::Terrain() {
    this->initialized = true;
}

void Terrain::init() {
    this->mesh.init();

    this->initialized = true;
}

void Terrain::render(Shader * shader) {
    if (!this->initialized) return;

    this->mesh.render(shader);
}

void Terrain::cleanUp() {
    if (!this->initialized) return;

    this->mesh.cleanUp();

    this->initialized = false;
}
