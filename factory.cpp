#include "render.hpp"

ModelFactory::ModelFactory(std::string & dir) {
    this->root = dir;
};

Model * ModelFactory::createModel(std::string file) {
    return new Model(this->root, file);
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

ModelFactory * ModelFactory::singleton = nullptr;
