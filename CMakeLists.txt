cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(osmview C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(SDL2PP_WITH_IMAGE ON ) # if you need SDL_image support
set(SDL2PP_WITH_MIXER OFF) # if you need SDL_mixer support
set(SDL2PP_WITH_TTF   ON ) # if you need SDL_ttf support
add_subdirectory(external/libSDL2pp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

add_executable(osmview
               src/osmview.cpp
               src/mapview.cpp
               src/mapview.hpp
               src/coord.cpp
               src/coord.hpp
               src/tilecache.cpp
               src/tilecache.hpp
               src/tilecacheitem.cpp
               src/tilecacheitem.hpp
               src/downloader.cpp
               src/downloader.hpp
               src/curl.cpp
               src/curl.hpp
               src/layer.cpp
               src/layer.hpp
               src/tilelayer.cpp
               src/tilelayer.hpp
               src/timer.cpp
               src/timer.hpp
               src/tile_id.cpp
               src/tile_id.hpp
               src/filesystem.cpp
               src/filesystem.hpp
               src/worker_pool.hpp
              )

target_link_libraries(osmview PRIVATE
    Threads::Threads
    CURL::libcurl
    SDL2pp::SDL2pp)
set_target_properties(osmview PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    )
target_compile_options(osmview PRIVATE -Wall -Wextra -pedantic)

if(USE_TSAN)
    target_compile_options(osmview PRIVATE -fsanitize=thread -O1 -g -fno-omit-frame-pointer)
    set_target_properties(osmview PROPERTIES LINK_FLAGS -fsanitize=thread)
endif()

if(USE_ASAN)
    target_compile_options(osmview PRIVATE -fsanitize=address -O1 -g -fno-omit-frame-pointer)
    set_target_properties(osmview PROPERTIES LINK_FLAGS -fsanitize=address)
endif()
