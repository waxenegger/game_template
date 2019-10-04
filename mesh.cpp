#include "render.hpp"

void Mesh::init() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    for (auto & texure : this->textures) {
        SDL_Surface * textureSurface = IMG_Load(texure.getPath().c_str());
        if(textureSurface != nullptr) {
            glGenTextures(1, &this->TEXTURE_ID);
            glBindTexture(GL_TEXTURE_2D, this->TEXTURE_ID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSurface->w, textureSurface->h, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, textureSurface->pixels);
        }
        SDL_FreeSurface(textureSurface);
    }

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
}

void Mesh::render() {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
}

void Mesh::cleanUp() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
    glDeleteTextures(1, &this->TEXTURE_ID);
}
