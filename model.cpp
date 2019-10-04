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
     std::vector<Vertex> vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture> textures;

     for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
         Vertex vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

         if (mesh->HasNormals() && mesh->mNormals->Length() == mesh->mNumVertices)
             vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

         if(mesh->HasTextureCoords(0)) {
             vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
         } else vertex.uv = glm::vec2(0.0f, 0.0f);

         if (mesh->HasTangentsAndBitangents()) {
             if (mesh->mTangents->Length() == mesh->mNumVertices)
                 vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

             if (mesh->mBitangents->Length() == mesh->mNumVertices)
                 vertex.bitTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
         }

         vertices.push_back(vertex);
     }

     for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
         const aiFace face = mesh->mFaces[i];
         for(unsigned int j = 0; j < face.mNumIndices; j++)
             indices.push_back(face.mIndices[j]);
     }

     if (scene->HasMaterials()) {
         const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

         this->addTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);
         this->addTextures(material, aiTextureType_SPECULAR, "texture_specular", textures);
         this->addTextures(material, aiTextureType_HEIGHT, "texture_normal", textures);
         this->addTextures(material, aiTextureType_AMBIENT, "texture_height", textures);
     }

     return Mesh(vertices, indices, textures);
}

void Model::addTextures(const aiMaterial * mat, const aiTextureType type, const std::string name, std::vector<Texture> & textures) {
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::map<std::string, Texture>::iterator val(TEXTURES.find(str.C_Str()));

        if (val != TEXTURES.end()) textures.push_back(val->second);
        else {
            Texture texture;
            texture.setType(name);
            texture.setPath(str.C_Str());
            textures.push_back(texture);
            TEXTURES[str.C_Str()] = texture;
        }
    }
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
