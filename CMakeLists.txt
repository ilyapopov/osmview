project(osmview C CXX)
cmake_minimum_required(VERSION 2.6)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not permitted. Make a separate folder for building:\nmkdir build; cd build; cmake ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)

SET(SDL2PP_WITH_IMAGE ON) # if you need SDL_image support
#SET(SDL2PP_WITH_MIXER ON) # if you need SDL_mixer support
SET(SDL2PP_WITH_TTF ON) # if you need SDL_ttf support
ADD_SUBDIRECTORY(deps/libSDL2pp)

add_subdirectory(src)

