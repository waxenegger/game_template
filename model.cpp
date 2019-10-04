#include "render.hpp"

Model::Model(const std::string & file) {
    this->id = file;
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(file.c_str(),
            aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene == nullptr) {
        std::cerr << importer.GetErrorString() << std::endl;
        return;
    }

    if (scene->HasMeshes()) {
        this->processNode(scene->mRootNode, scene);
        this->loaded = true;
    } else std::cerr << "Model does not contain meshes" << std::endl;
}

void Model::processNode(const aiNode * node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
    // data to fill
     std::vector<Vertex> vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture> textures;

     for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
         Vertex vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

         vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

         if(mesh->mTextureCoords[0]) {
             vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
         } else vertex.uv = glm::vec2(0.0f, 0.0f);

         vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

         vertex.tangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

         vertices.push_back(vertex);
     }

     for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
         const aiFace face = mesh->mFaces[i];
         for(unsigned int j = 0; j < face.mNumIndices; j++)
             indices.push_back(face.mIndices[j]);
     }

     /*
      * TODO: materials
     aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

     vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
     textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

     vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
     textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

     std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
     textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

     std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
     textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
     */

     return Mesh(vertices, indices, textures);
}

void Model::init() {
    if (!this->loaded) return;

    for (auto & mesh : this->meshes) mesh.init();

    this->initialized = true;
}

void Model::render() {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.render();
}

void Model::cleanUp() {
    if (!this->initialized) return;

    for (auto & mesh : this->meshes) mesh.cleanUp();

    this->initialized = false;
    this->loaded = false;
}
