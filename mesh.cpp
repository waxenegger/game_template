#include "render.hpp"

void Mesh::init() {
    glGenVertexArrays(1, &this->VAO);

    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    glGenBuffers(1, &this->MODEL_MATRIX);
    glGenBuffers(1, &this->MATERIALS);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

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
        //glGenerateMipmap(GL_TEXTURE_2D);
        i++;
    }
}

void Mesh::setModelMatrices(std::vector<glm::mat4> & modelMatrices) {
    if (modelMatrices.size() == this->modelMatrices.size()) return;

    this->modelMatrices = modelMatrices;

    glBindBuffer(GL_ARRAY_BUFFER, this->MODEL_MATRIX);
    glBufferData(GL_ARRAY_BUFFER, this->modelMatrices.size() * sizeof(glm::mat4), &this->modelMatrices[0], GL_DYNAMIC_DRAW);

    if (!this->modelMatricesEnabled) {
        glBindVertexArray(this->VAO);

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);

        this->modelMatricesEnabled = true;
        glBindVertexArray(0);
    }
}

void Mesh::setMaterials(std::vector<Material> & materials) {
    if (materials.size() == this->materials.size()) return;

    this->materials = materials;

    glBindBuffer(GL_ARRAY_BUFFER, this->MATERIALS);
    glBufferData(GL_ARRAY_BUFFER, this->materials.size() * sizeof(Material), &this->materials[0], GL_DYNAMIC_DRAW);

    if (!this->materialsEnabled) {
        glBindVertexArray(this->VAO);

        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, emissiveColor));

        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, ambientColor));

        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, diffuseColor));

        glEnableVertexAttribArray(12);
        glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, specularColor));

        glEnableVertexAttribArray(13);
        glVertexAttribPointer(13, 1, GL_FLOAT, GL_FALSE, sizeof(Material), (void*)offsetof(Material, shininess));

        glVertexAttribDivisor(9, 1);
        glVertexAttribDivisor(10, 1);
        glVertexAttribDivisor(11, 1);
        glVertexAttribDivisor(12, 1);
        glVertexAttribDivisor(13, 1);

        this->materialsEnabled = true;
        glBindVertexArray(0);
    }
}

void Mesh::render(Shader * shader) {
    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->MODEL_MATRIX);
    glBufferSubData(GL_ARRAY_BUFFER, 0, this->modelMatrices.size() * sizeof(glm::mat4), &this->modelMatrices[0]);

    glBindBuffer(GL_ARRAY_BUFFER, this->MATERIALS);
    glBufferSubData(GL_ARRAY_BUFFER, 0, this->materials.size() * sizeof(Material), &this->materials[0]);

    if (shader != nullptr && shader->isBeingUsed()) {
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

    glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0, this->modelMatrices.size());

    glBindVertexArray(0);
}

void Mesh::cleanUp() {
    for (int i=0;i<14;i++) glDisableVertexAttribArray(i);

    glDeleteVertexArrays(1, &this->VAO);

    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);

    glDeleteBuffers(1, &this->MODEL_MATRIX);
    glDeleteBuffers(1, &this->MATERIALS);

    for (auto texture : this->textures) texture->cleanUp();
}
