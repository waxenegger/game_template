#include "render.hpp"

ModelFactory::ModelFactory(std::string & dir) {
    this->root = dir;
};

Model * ModelFactory::createModel(std::string file) {
    std::string fullyQualifiedName(this->root + file);
    std::map<std::string, Model *>::iterator val(this->MODELS.find(fullyQualifiedName));

    Model * ret = nullptr;

    if (val != MODELS.end()) ret = val->second;
    else {
        ret = new Model(this->root, file);
        if (ret->hasBeenLoaded()) {
            ret->init();
            this->MODELS[fullyQualifiedName] = ret;
        } else {
            delete ret;
            ret = nullptr;
        }
    }

    return ret;
};

Image * ModelFactory::createTextImage(std::string text, std::string font, int size) {
    Image * ret = Image::fromText(std::string(this->root + "/res/fonts/" + font), text, size);
    ret->useShader(new Shader(this->root + "/res/shaders/textures"));
    return ret;
};

Image * ModelFactory::createImage(std::string file) {
    Image * ret = Image::fromFile(std::string(this->root + file));
    ret->useShader(new Shader(this->root + "/res/shaders/textures"));
    return ret;
};

ModelFactory::~ModelFactory() {
    for (auto & modelEntry : this->MODELS) delete modelEntry.second;
    this->MODELS.clear();
}

ModelFactory * ModelFactory::singleton = nullptr;
