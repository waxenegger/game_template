#ifndef INCLUDES_HPP
#define INCLUDES_HPP
    #ifdef WIN32
        #include <windows.h>
        #include <gl\gl.h>
        #include <gl/glu.h>
    #endif

    #include <iostream>
    #include <fstream>
    #include <ctype.h>
    #include <memory>
    #include <vector>
    #include <map>
    #include <time.h>

    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>

    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include <glm/gtx/string_cast.hpp>

    #include <GL/glew.h>

    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>

#endif
