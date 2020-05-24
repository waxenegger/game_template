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

    int i=0;
    for (auto & texture : this->textures) {
        if (!texture->isValid()) continue;

        SDL_Surface * textureSurface = texture->getTextureSurface();

        GLuint id = 0;
        glGenTextures(1, &id);
        // set global id
        texture->setId(id);

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture->getId());

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSurface->w, textureSurface->h, 0, texture->getImageFormat(),
                GL_UNSIGNED_BYTE, textureSurface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        i++;
    }

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
}

void Mesh::render(Shader * shader) {
    glBindVertexArray(this->VAO);

    if (shader != nullptr && shader->isBeingUsed()) {
        shader->setVec4("ambientMaterial", this->material.ambientColor);
        shader->setVec4("diffuseMaterial", this->material.diffuseColor);
        shader->setVec4("specularMaterial", this->material.specularColor);
        shader->setFloat("shininess", this->material.shininess);

        int i=0;
        for (auto & texture : this->textures) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture->getId());

            shader->setInt(texture->getType(), i);
            shader->setInt("has_" + texture->getType(),
                    texture->getType() == Model::TEXTURE_NORMALS && !this->useNormalsTexture ? 0 : 1);
            i++;
        }
    }

    // instanced part - start
    glGenBuffers(1, &this->MODEL_MATRIX);
    glBindBuffer(GL_ARRAY_BUFFER, this->MODEL_MATRIX);
    glBufferData(GL_ARRAY_BUFFER, this->modelMatrices.size() * sizeof(glm::mat4), &this->modelMatrices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    // instanced part - end


    glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0, this->modelMatrices.size());
    //glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::cleanUp() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);
    glDisableVertexAttribArray(8);

    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
    glDeleteBuffers(1, &this->MODEL_MATRIX);
    for (auto texture : this->textures) texture->cleanUp();
}
