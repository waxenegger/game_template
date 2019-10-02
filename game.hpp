#ifndef GAME_HPP
#define GAME_HPP

    #include "includes.hpp"
    #include "render.hpp"

	static const int DEFAULT_WIDTH = 640;
	static const int DEFAULT_HEIGHT = 480;

	class Camera final {
		private:
			Shader shader = Shader("/opt/projects/opengl/res/test");
			glm::vec3 position = glm::vec3 (1.0f);
			glm::vec3 direction = glm::vec3 (0.001f,0.0f,0.001f);
			glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
					static_cast<float>(DEFAULT_WIDTH) / static_cast<float>(DEFAULT_HEIGHT), 0.01f, 1000.0f);
			const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		public:
			Camera() {};
			Camera(const float x, const float y, const float z) {
				this->position = glm::vec3(x, y, z);
			};
			void setPosition(const float x, const float y, const float z) {
				this->position = glm::vec3(x, y, z);
			};
			glm::vec3 getPosition() { return this->position; };
			void setDirection(const float x, const float y, const float z) {
				this->direction = glm::normalize(glm::vec3(x, y, z));
			};
			glm::vec3 getDirection() { return this->direction; };
			void setPerspective(const glm::mat4 perspective) {
				this->perspective = perspective;
			}
			void update() {
			    this->shader.use();
				this->shader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(1.0f,1.0f,1.0f)));
				this->shader.setMat4("view", glm::lookAt(this->position, this->position + this->direction, this->upVector));
				this->shader.setMat4("projection", this->perspective);
				this->shader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
				this->shader.setVec3("objectColor2", 1.0f, 1.0f, 0.0f);
			}
	};

    class Game {
		private:
			int width = DEFAULT_WIDTH;
			int height = DEFAULT_HEIGHT;

			Uint32 frameStart = 0;
			Uint32 frameDuration = 0;

			bool wireframe = false;

			SDL_Window * window = nullptr;
			SDL_GLContext glContext = nullptr;

			std::vector<Entity> scene;

			void clearScreen(float r, float g, float b, float a);
			void render(Camera & camera);
			void cleanUp();

		public:
			bool init();
			void run();
			float getAspectRatio() const;
			void toggleWireframe();
			void resize(int width, int height);
			float getLastFrameDuration() const;
			virtual ~Game();
	};

#endif
