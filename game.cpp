#include "game.hpp"

Game::Game(std::string root) {
    this->root = root;
    if (this->root[static_cast<int>(root.length())-1] != '/') this->root.append("/");
}

/*
 * Cleans Up Resources:
 * - OpenGL Context
 * - Main Window
 * - SDL2
 */
void Game::cleanUp() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/*
 * Initializes the Game doing the following:
 * 1. Init SDL2
 * 2. Create Main Window
 * 3. Create OpenGl Context
 * 4. Init GLEW
 */
bool Game::init() {
    if (SDL_Init( SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError()
                        << std::endl;
        return false;
    } else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);
        //glDepthMask( GL_FALSE );
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_CULL_FACE);
        //glEnable(GL_AUTO_NORMAL);

        window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, this->width, this->height,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (window == nullptr) {
            std::cerr << "Window could not be created! SDL Error: "
                    << SDL_GetError() << std::endl;
            return false;
        } else {
            glContext = SDL_GL_CreateContext(window);
            if (glContext == nullptr) {
                std::cerr << "OpenGL context could not be created! SDL Error: "
                        << SDL_GetError() << std::endl;
                return false;
            } else {
                if (SDL_GL_SetSwapInterval(1) < 0) {
                    std::cerr << "Warning: Unable to set VSync! SDL Error: "
                            << SDL_GetError() << std::endl;
                }
                glewExperimental = GL_TRUE;
                GLenum err = glewInit();
                if (GLEW_OK != err) {
                    std::cerr << "GLEW could not be initialized! Error: "
                            << glewGetErrorString(err) << std::endl;
                }
            }
        }
    }

    return true;
}

void Game::run() {
    bool quit = false;

    SDL_Event e;

    SDL_StartTextInput();
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if (this->terrain == nullptr) {
        this->terrain = new Terrain(this->root);
        this->terrain->init();

    }
    this->createTestModels();

    while (!quit) {

        while (SDL_PollEvent(&e) != 0) {
            switch(e.type) {

            case SDL_MOUSEBUTTONUP:
                SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() == SDL_TRUE ? SDL_FALSE : SDL_TRUE);
                break;

            case SDL_MOUSEMOTION:
                this->camera->updateDirection(
                        e.motion.xrel, e.motion.yrel, this->frameDuration);
                break;

            case SDL_MOUSEWHEEL:
            {
                const Sint32 delta = e.wheel.y * (e.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1);
                float newFovy = this->camera->getFieldOfViewY() - delta * 2;
                if (newFovy < 1) newFovy = 1;
                else if (newFovy > 45) newFovy = 45;
                this->camera->setFieldOfViewY(newFovy);
                this->camera->setPerspective(
                        glm::perspective(glm::radians(this->camera->getFieldOfViewY()),
                                this->getAspectRatio(), 0.01f, 1000.0f));
                break;
            }
            case SDL_WINDOWEVENT:
                if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    this->resize(e.window.data1, e.window.data2);
                    this->camera->setPerspective(
                            glm::perspective(glm::radians(this->camera->getFieldOfViewY()),
                                    this->getAspectRatio(), 0.01f, 1000.0f));
                }
                break;

            case SDL_QUIT:
                quit = true;
                break;

            case SDL_TEXTINPUT:
                char input = toupper(e.text.text[0]);
                if (input == 'Q') quit = true;
                else if (input == 'F') this->toggleWireframe();
                else if (input == '+') this->world->setAmbientLightFactor(this->world->getAmbientLight().x + 0.1);
                else if (input == '-') this->world->setAmbientLightFactor(this->world->getAmbientLight().x - 0.1);
                else if (input == '*') this->world->setSunLightStrength(this->world->getSunLightColor().x + 0.1);
                else if (input == '/') this->world->setSunLightStrength(this->world->getSunLightColor().x - 0.1);
                else this->camera->updateLocation(input, this->frameDuration);

                break;
            }
        }

        this->render();
    }

    this->terrain->cleanUp();
    for (auto & entity : this->scene) entity->cleanUp();

    SDL_StopTextInput();
}

void Game::clearScreen(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,(GLsizei)this->width,(GLsizei)this->height);
}

void Game::render() {
    glPolygonMode(GL_FRONT_AND_BACK, this->wireframe ? GL_LINE : GL_FILL);

    this->clearScreen(0, 0, 0, 0);

    this->terrain->render();
    for (auto & entity : this->scene) entity->render();

    SDL_GL_SwapWindow(window);

    const Uint32 currentTime = SDL_GetTicks();
    this->frameDuration = currentTime - this->frameStart;
    this->frameStart = currentTime;
}

float Game::getAspectRatio() const {
    return static_cast<float>(this->width) / static_cast<float>(this->height);
}

void Game::toggleWireframe() {
    this->wireframe = !this->wireframe;
}


void Game::resize(int width, int height) {
    this->width = width;
    this->height = height;

    glViewport(0,0,(GLsizei)this->width,(GLsizei)this->height);
}

float Game::getLastFrameDuration() const {
    return (float) this->frameDuration / 1000.0f;
}

Game::~Game() {
    if (this->terrain != nullptr) delete this->terrain;
    if (this->camera != nullptr) delete this->camera;
    if (this->world != nullptr) delete this->world;
    cleanUp();
}

void Game::createTestModels() {
    Model * teapot = new Model(this->root, "test/utah-teapot.obj");
    if (teapot->hasBeenLoaded()) {
        teapot->init();
        Entity * ent = new Entity(teapot, new Shader());
        ent->setScaleFactor(1.0f);
        ent->setColor(1.0f, 1.0f, 0.0f, 1.0f);
        this->scene.push_back(std::unique_ptr<Entity>(ent));
    }
}

int main(int argc, char **argv) {
    Game game((argc > 1) ? std::string(argv[1]) : "");
    if (game.init()) game.run();

    TEXTURES.clear();

    return 0;
}
