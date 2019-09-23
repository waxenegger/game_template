#include <iostream>
#include <ctype.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class Game {
  private:
    SDL_Window * window = nullptr;
    SDL_GLContext glContext = nullptr;

    void render() {}

    void cleanUp() {
      SDL_GL_DeleteContext(glContext);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }

  public:

    bool init() {
      if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
	      std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
      } else {
        // OpenGL version
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( window == nullptr ) {
	  std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
          return false;
        } else {
          glContext = SDL_GL_CreateContext( window );
          if( glContext == nullptr ) {
	    std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
            return false;
          } else {
            if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
	      std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
            }
	    GLenum err = glewInit();
	    if (GLEW_OK != err) {
              std::cout << "GLEW could not be intialized! Error: " << glewGetErrorString(err) << std::endl;
	    }
          }
        }
      }

      return true;
    }

    void run() {
      bool quit = false;

      SDL_Event e;

      SDL_StartTextInput();

      while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
          if( e.type == SDL_QUIT ||
              ( e.type == SDL_TEXTINPUT && toupper(e.text.text[ 0 ]) == 'Q') ) {
            quit = true;
          }
        }

        this->render();

        SDL_GL_SwapWindow( window );
      }

      //Disable text input
      SDL_StopTextInput();
    }

    ~Game() {
      cleanUp();
    }
};

int main(int argc, char **argv) {
  Game game;

  if (game.init()) game.run();

  return 0;
}
