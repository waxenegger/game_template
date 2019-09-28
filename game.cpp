#include <game.hpp>

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
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError()
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
            std::cout << "Window could not be created! SDL Error: "
                    << SDL_GetError() << std::endl;
            return false;
        } else {
            glContext = SDL_GL_CreateContext(window);
            if (glContext == nullptr) {
                std::cout << "OpenGL context could not be created! SDL Error: "
                        << SDL_GetError() << std::endl;
                return false;
            } else {
                if (SDL_GL_SetSwapInterval(1) < 0) {
                    std::cout << "Warning: Unable to set VSync! SDL Error: "
                            << SDL_GetError() << std::endl;
                }
                glewExperimental = GL_TRUE;
                GLenum err = glewInit();
                if (GLEW_OK != err) {
                    std::cout << "GLEW could not be initialized! Error: "
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

	Renderable obj;
	obj.loadMeshFromObjectFile("/opt/projects/opengl/res/kitchen/upper cabinet 60x35x60 DGD.obj");
    this->scene.push_back(obj);

    Shader shader("/opt/projects/opengl/res/test");
    shader.use();

    glm::vec3 camPosition(0.0f, 0.0f, 3.0f);
    glm::vec3 camDirection(0.0f, 0.0f, 0.0f);
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f,0.7f,0.7f));
	glm::mat4 view = glm::lookAt(camPosition, camDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", glm::perspective(glm::radians(45.0f), this->getAspectRatio(), 0.01f, 100.f));
	shader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
	shader.setVec3("objectColor", 1.0f, 1.0f, 0.0f);

    float pitch = 0;
	float yaw = 0;
	bool is_dragging = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
			switch(e.type) {
				case SDL_MOUSEBUTTONDOWN:
					if (e.button.button != SDL_BUTTON_LEFT) break;
					is_dragging = true;
					break;
			case SDL_MOUSEBUTTONUP:
				is_dragging = false;
				break;
			case SDL_MOUSEMOTION:
					if (!is_dragging) break;

					pitch -= e.motion.yrel * 0.05;
					yaw += e.motion.xrel * 0.05;

					camPosition.y = cos(pitch) * cos(yaw);
					camPosition.x = sin(pitch);
					camPosition.z = std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
					camPosition = normalize(camPosition);
					//camPosition.z = 3.0f;
					view = glm::lookAt(camPosition, camDirection, glm::vec3(0.0f, 1.0f, 0.0f));
					shader.setMat4("view", view);

					break;
				case SDL_WINDOWEVENT:
					if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						this->resize(e.window.data1, e.window.data2);
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_TEXTINPUT:
					char input = toupper(e.text.text[0]);
					if (input == 'Q') quit = true;
					else if (input == 'W')this->toggleWireframe();
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

	this->clearScreen(0, 0, 0, 0);

	for (auto & renderable : this->scene) renderable.render();

	SDL_GL_SwapWindow(window);
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

Game::~Game() {
    cleanUp();
}

int main(int argc, char **argv) {
    Game game;

    if (game.init())
        game.run();

    return 0;
}
