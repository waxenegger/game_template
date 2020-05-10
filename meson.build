project('game', 'cpp', default_options : ['cpp_std=c++14', 'werror=true'])

WINDOWS_SDL_PATH = join_paths(meson.source_root(),'SDL2-2.0.10')
WINDOWS_SDL_IMAGE_PATH = join_paths(meson.source_root(),'SDL2_image-2.0.5')
WINDOWS_SDL_FONT_PATH = join_paths(meson.source_root(),'SDL2_ttf-2.0.15')
WINDOWS_GLEW_PATH = join_paths(meson.source_root(),'glew-2.1.0')
WINDOWS_ASSIMP_PATH = join_paths(meson.source_root(),'assimp')
WINDOWS_KITS = 'C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.18362.0\\um\\x64'

includeDir = []
dependencies = []

sdlDep = dependency('SDL2', required: false)

if sdlDep.found()
   sdlImageDep = dependency('SDL2_image')
   sdlFontDep = dependency('SDL2_ttf')
   glewDep = dependency('glew')
   assimpDep = dependency('assimp')
   dependencies = [sdlDep, sdlImageDep, sdlFontDep, glewDep, assimpDep]
else
   openGlDepWin = meson.get_compiler('cpp').find_library('OpenGL32', dirs: [WINDOWS_KITS])
   sdlIncDir = join_paths(WINDOWS_SDL_PATH, 'include')
   sdlLibDir = join_paths(WINDOWS_SDL_PATH, 'lib', 'x64')
   sdlImageIncDir = join_paths(WINDOWS_SDL_IMAGE_PATH, 'include')
   sdlImageLibDir = join_paths(WINDOWS_SDL_IMAGE_PATH, 'lib', 'x64')
   sdlFontIncDir = join_paths(WINDOWS_SDL_FONT_PATH, 'include')
   sdlFontLibDir = join_paths(WINDOWS_SDL_FONT_PATH, 'lib', 'x64')   
   glewIncDir = join_paths(WINDOWS_GLEW_PATH, 'include')
   glewLibDir = join_paths(WINDOWS_GLEW_PATH, 'lib', 'x64')
   assimpIncDir = join_paths(WINDOWS_ASSIMP_PATH, 'include')
   assimpLibDir = join_paths(WINDOWS_ASSIMP_PATH, 'lib')
   sdlDepWin = meson.get_compiler('cpp').find_library('SDL2', dirs: [sdlLibDir])
   sdlDep2Win = meson.get_compiler('cpp').find_library('SDL2main', dirs: [sdlLibDir])
   sdlImageDepWin = meson.get_compiler('cpp').find_library('SDL2_image', dirs: [sdlImageLibDir])
   sdlFontDepWin = meson.get_compiler('cpp').find_library('SDL2_ttf', dirs: [sdlFontLibDir])
   glewDepWin = meson.get_compiler('cpp').find_library('glew32', dirs: [glewLibDir])
   assimpDepWin = meson.get_compiler('cpp').find_library('assimp-vc141-mtd', dirs: [assimpLibDir])

   dependencies = [openGlDepWin, sdlDepWin, sdlDep2Win, sdlImageDepWin, sdlFontDepWin, glewDepWin, assimpDepWin]
   includeDir = [sdlIncDir, sdlImageIncDir, sdlFontIncDir, glewIncDir, assimpIncDir]

endif

src = [ 'world.cpp', 'camera.cpp', 'mesh.cpp', 'terrain.cpp', 'model.cpp', 
		'entity.cpp', 'shader.cpp', 'factory.cpp', 'image.cpp' ]

executable('game', 'game.cpp', src, include_directories: includeDir, dependencies: dependencies) 
