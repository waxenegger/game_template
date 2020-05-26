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

    if (this->terrain != nullptr) this->terrain->cleanUp();
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

    const Uint32 currentTime = SDL_GetTicks();
    const Uint32 diff = this->forceFixedFrameRate ? (currentTime - this->frameDrawn) : 0;

    if (diff == 0 || diff > FIXED_DRAW_INTERVAL) {
        const Uint32 frameStart = SDL_GetTicks();
        this->clearScreen(0, 0, 0, 1);

        if (this->terrain != nullptr) this->terrain->render();

        for (auto & entity : this->scene) entity->render();

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
    if (this->terrain != nullptr) delete this->terrain;
    if (this->camera != nullptr) delete this->camera;
    if (this->world != nullptr) delete this->world;
    if (this->factory != nullptr) delete this->factory;
    cleanUp();
}

void Game::createTestModels() {

    std::shared_ptr<Model> model(this->factory->createModel("/res/models/teapot.obj"));
    if (model->hasBeenLoaded()) {
        model->useNormalsTexture(false);
        model->init();

        for (int x = 0;x< 1; x++) {
            Entity * ent = new Entity(model);
            ent->useShader(new Shader(this->root + "/res/shaders/textures"));
            ent->setPosition(4.0f, 0.0f, -15.0f);
            ent->setRotation(0, -45, 0);
            ent->setScaleFactor(2.0f);

            for (int j=0;j<50;j++) {
                glm::vec3 pos = ent->getPosition();
                ent->setPosition(pos.x + 10 , pos.y, pos.z);
                ent->setColor(0.02f * j, 0.02f * j, 0.02f * j, 1.0f);
                //ent->setColor(0.0, 0.0f, 1.0f, 1.0f);
                model->addModelInstance(ent->calculateTransformationMatrix());
            }

            ent->setPosition(4.0f, 0.0f, -15.0f);

            this->scene.push_back(std::unique_ptr<Renderable>(std::move(ent)));
        }
    }

    /*
    for (int x = 0;x < 1; x++) {
        std::unique_ptr<Renderable> img(this->factory->createImage("/res/models/rock.png"));
        img->setPosition(4.0f + 10 * x, 0.0f, -15.0f);
        img->setRotation(0, -45, 0);
        img->setScaleFactor(2.0f);
        this->scene.push_back(std::move(img));
    }
    */
}
std::map<std::string, std::unique_ptr<Texture> > Game::TEXTURES;


int main(int argc, char **argv) {
    Game game((argc > 1) ? std::string(argv[1]) : "./");
    if (game.init()) game.run();

    Game::TEXTURES.clear();

    TTF_Quit();

    return 0;
}

