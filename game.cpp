#include "game.hpp"

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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

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

	Camera camera = Camera(-5.0f, 0.0f, -5.0f);

    Model m("/opt/projects/opengl/res/test.obj");
    if (m.hasBeenLoaded()) {
        m.init();
        this->scene.push_back(Entity(m, Shader()));
    }

	float pitch = glm::asin(1);
	float yaw = glm::atan(0.0f, 0.0f);

    while (!quit) {
    	//glm::mat4 viewInverted = glm::inverse(view);
		//glm::vec3 directionInverted = - glm::normalize(glm::vec3(viewInverted[2]));

		const glm::vec3 presentDirection = camera.getDirection();
		glm::vec3 presentPosition = camera.getPosition();
		pitch = glm::asin(presentDirection.y);
		yaw   = glm::atan(presentDirection.z, presentDirection.x);

		//pitch = glm::asin(directionInverted.y);
		//yaw   = glm::atan(directionInverted.z, directionInverted.x);

    	while (SDL_PollEvent(&e) != 0) {
			switch(e.type) {
				case SDL_MOUSEBUTTONUP:
					SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() == SDL_TRUE ? SDL_FALSE : SDL_TRUE);
					break;
				case SDL_MOUSEMOTION:
					pitch -= e.motion.yrel * 0.0005 * this->frameDuration;
					yaw += e.motion.xrel * 0.0005 * this->frameDuration;
					camera.setDirection(
							glm::cos(pitch) * glm::cos(yaw), glm::sin(pitch), glm::cos(pitch) * glm::sin(yaw));
					break;
				case SDL_WINDOWEVENT:
					if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						this->resize(e.window.data1, e.window.data2);
						camera.setPerspective(
							glm::perspective(glm::radians(45.0f), this->getAspectRatio(), 0.01f, 1000.0f));
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_TEXTINPUT:
					char input = toupper(e.text.text[0]);
					if (input == 'Q') quit = true;
					else if (input == 'F')this->toggleWireframe();
					else {
						float speedFactor = 0.05 * this->frameDuration;

						float deltaXRot = glm::cos(pitch) * glm::cos(yaw);
						float deltaYRot = glm::cos(pitch) * glm::sin(yaw);

						float deltaXMove = glm::cos(yaw - glm::pi<float>() / 2);
						float deltaYMove = glm::sin(yaw - glm::pi<float>() / 2);

						if (input == 'W') {
							presentPosition.x += deltaXRot * speedFactor;
							presentPosition.z += deltaYRot * speedFactor;
						} else if (input == 'S') {
							presentPosition.x -= deltaXRot * speedFactor;
							presentPosition.z -= deltaYRot * speedFactor;
						} else if (input == 'A') {
							presentPosition.x += deltaXMove * speedFactor;
							presentPosition.z += deltaYMove * speedFactor;
						} else if (input == 'D') {
							presentPosition.x -= deltaXMove * speedFactor;
							presentPosition.z -= deltaYMove * speedFactor;
						}

						camera.setPosition(presentPosition.x, presentPosition.y, presentPosition.z);
						//view = glm::lookAt(camPosition, camPosition + directionInverted, upVector);
						//shader.setMat4("view", view);
					}
				break;
			}
        }

        this->render(camera);
    }

    for (auto & entity : this->scene) entity.cleanUp();

    SDL_StopTextInput();
}

void Game::clearScreen(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,(GLsizei)this->width,(GLsizei)this->height);
}

void Game::render(Camera & camera) {
	glPolygonMode(GL_FRONT_AND_BACK, this->wireframe ? GL_LINE : GL_FILL);

	this->clearScreen(0, 0, 0, 0);

	camera.update();

	for (auto & entity : this->scene) entity.render();

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
    cleanUp();
}

int main(int argc, char **argv) {
    Game game;

    if (game.init())
        game.run();

    return 0;
}
