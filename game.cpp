#include "game.hpp"

Game::Game(std::string root) {
    this->root = root;
    if (this->root[static_cast<int>(root.length())-1] != '/') this->root.append("/");
    this->factory = ModelFactory::instance(this->root);

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
        SDL_GL_SetSwapInterval(1);

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
                glEnable(GL_DEPTH_TEST);

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

    TTF_Init();

    this->state = new GameState(this->root);
    this->state->init();

    return true;
}

void Game::run() {
    bool quit = false;

    SDL_Event e;

    SDL_StartTextInput();
    SDL_SetRelativeMouseMode(SDL_TRUE);

    this->createTestModels();

    while (!quit) {

        while (SDL_PollEvent(&e) != 0) {
            switch(e.type) {

            case SDL_MOUSEBUTTONUP:
                SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() == SDL_TRUE ? SDL_FALSE : SDL_TRUE);
                break;

            case SDL_MOUSEMOTION:
                if (SDL_GetRelativeMouseMode() == SDL_TRUE)
                    this->camera->updateDirection(
                            static_cast<float>(e.motion.xrel),
                            static_cast<float>(e.motion.yrel),
                            static_cast<float>(FIXED_DRAW_INTERVAL));
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

                switch (input) {
                    case 'Q':
                        quit = true;
                        break;
                    case 'F':
                        this->toggleWireframe();
                        break;
                    case '+':
                        this->world->setAmbientLightFactor(this->world->getAmbientLight().x + 0.1);
                        break;
                    case '-':
                        this->world->setAmbientLightFactor(this->world->getAmbientLight().x - 0.1);
                        break;
                    case '*':
                        this->world->setSunLightStrength(this->world->getSunLightColor().x + 0.1);
                        break;
                    case '/':
                        this->world->setSunLightStrength(this->world->getSunLightColor().x - 0.1);
                        break;
                    default:
                        if (SDL_GetRelativeMouseMode() == SDL_TRUE)
                            this->camera->updateLocation(input, static_cast<float>(FIXED_DRAW_INTERVAL));
                }
                break;
            }
        }

        this->render();
    }

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

    const Uint32 currentTime = SDL_GetTicks();
    const Uint32 diff = this->forceFixedFrameRate ? (currentTime - this->frameDrawn) : 0;

    if (diff == 0 || diff > FIXED_DRAW_INTERVAL) {
        const Uint32 frameStart = SDL_GetTicks();
        this->clearScreen(0, 0, 0, 1);

        this->state->render();

        SDL_GL_SwapWindow(window);

        this->frameDrawn = SDL_GetTicks();
        this->frameDuration = this->frameDrawn - frameStart;
    }
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
    if (this->camera != nullptr) delete this->camera;
    if (this->world != nullptr) delete this->world;
    if (this->factory != nullptr) delete this->factory;
    if (this->state != nullptr) delete this->state;
    cleanUp();
}

void Game::createTestModels() {

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Model * sunModel(this->factory->createModel("/res/models/sun.obj"));
    if (sunModel->hasBeenLoaded()) {
        sunModel->useNormalsTexture(false);
        sunModel->init();
        Entity * sun = new Entity(sunModel);
        sun->setColor(1.0f, 1.0f, 0.0f, 1.0f);
        sun->setPosition(World::instance()->getSunDirection());
        sun->setScaleFactor(1.0f);

        this->state->addRenderable(sun);
    }

    Model * teapotModel(this->factory->createModel("/res/models/teapot.obj"));
    if (teapotModel->hasBeenLoaded()) {
        teapotModel->useNormalsTexture(true);
        teapotModel->init();

        for (int j=0;j<100;j++) {
            Entity * teapot = new Entity(teapotModel);
            teapot->setColor(0.0f, 0.0f, 1.0f, 1.0);
            teapot->setPosition(4.0f + 10*j, 0.0f, -15.0f);
            teapot->setRotation(0, -90, 0);
            teapot->setScaleFactor(2.0f);
            this->state->addRenderable(teapot);
        }
    }

    Model * nanosuitModel(this->factory->createModel("/res/models/nanosuit.obj"));
    if (nanosuitModel->hasBeenLoaded()) {
        nanosuitModel->useNormalsTexture(true);
        nanosuitModel->init();

        for (int j=0;j<100;j++) {
            Entity * nanosuit = new Entity(nanosuitModel);
            nanosuit->useShader(new Shader(this->root + "/res/shaders/textures"));
            nanosuit->setColor(1.0f,1.0f,1.0f,1.0f);
            nanosuit->setPosition(4.0f + 10*j, 0.0f, -15.0f);
            nanosuit->setScaleFactor(1.0f);
            this->state->addRenderable(nanosuit);
        }
    }


    Renderable * rock = this->factory->createImage("/res/models/rock.png");
    rock->setPosition(4.0f, 0.0f, -15.0f);
    rock->setRotation(0, -90, 0);
    rock->setScaleFactor(2.0f);
    this->state->addRenderable(rock);
}
std::map<std::string, std::shared_ptr<Texture>> Game::TEXTURES;


int main(int argc, char **argv) {
    Game game((argc > 1) ? std::string(argv[1]) : "./");
    if (game.init()) game.run();

    Game::TEXTURES.clear();

    TTF_Quit();

    return 0;
}

