project('game', 'cpp')

WINDOWS_SDL_PATH = join_paths(meson.source_root(),'SDL2-2.0.10')
WINDOWS_GLEW_PATH = join_paths(meson.source_root(),'glew-2.1.0')

includeDir = []
dependencies = []

sdlDep = dependency('SDL2', required: false)

if sdlDep.found()
   glewDep = dependency('glew')
   dependencies = [sdlDep, glewDep]
else
sdlIncDir = join_paths(WINDOWS_SDL_PATH, 'include')
   sdlLibDir = join_paths(WINDOWS_SDL_PATH, 'lib', 'x64')
   glewIncDir = join_paths(WINDOWS_GLEW_PATH, 'include')
   glewLibDir = join_paths(WINDOWS_GLEW_PATH, 'lib', 'x64')
   sdlDepWin = meson.get_compiler('cpp').find_library('SDL2', dirs: [sdlLibDir])
   sdlDep2Win = meson.get_compiler('cpp').find_library('SDL2main', dirs: [sdlLibDir])
   glewDepWin = meson.get_compiler('cpp').find_library('glew32', dirs: [glewLibDir])

   dependencies = [sdlDepWin, sdlDep2Win, glewDepWin]
   includeDir = [sdlIncDir, glewIncDir]

endif

executable('game', 'game.cpp', include_directories: includeDir, dependencies: dependencies) 

