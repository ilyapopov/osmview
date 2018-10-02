cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(osmview C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(SDL2PP_WITH_IMAGE ON ) # if you need SDL_image support
set(SDL2PP_WITH_MIXER OFF) # if you need SDL_mixer support
set(SDL2PP_WITH_TTF   ON ) # if you need SDL_ttf support
add_subdirectory(deps/libSDL2pp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_executable(osmview
               src/osmview.cpp
               src/mapview.cpp
               src/coord.cpp
               src/tilecache.cpp
               src/tilecacheitem.cpp
               src/downloader.cpp
               src/curl.cpp
               src/layer.cpp
               src/tilelayer.cpp
               src/timer.cpp
               src/tile_id.cpp
               src/filesystem.cpp
              )

target_link_libraries(osmview PRIVATE Threads::Threads Boost::filesystem
    ${SDL2PP_LIBRARIES} ${SDLMAIN_LIBRARY} ${CURL_LIBRARIES})
target_include_directories(osmview SYSTEM PRIVATE ${SDL2PP_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})
set_target_properties(osmview PROPERTIES
    CXX_STANDARD 14
    CXX_EXTENSIONS OFF
    #CXX_INCLUDE_WHAT_YOU_USE "iwyu;-I/usr/lib/llvm-4.0/lib/clang/4.0.0/include/"
    #CXX_CLANG_TIDY "clang-tidy;-checks=*,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-pro-type-union-access,-readability-redundrant-declaration"
    )
target_compile_options(osmview PRIVATE -Wall -Wextra -Wpedantic)

if(USE_TSAN)
    target_compile_options(osmview PRIVATE -fsanitize=thread -O1 -g -fno-omit-frame-pointer)
    set_target_properties(osmview PROPERTIES LINK_FLAGS -fsanitize=thread)
endif()

if(USE_ASAN)
    target_compile_options(osmview PRIVATE -fsanitize=address -O1 -g -fno-omit-frame-pointer)
    set_target_properties(osmview PROPERTIES LINK_FLAGS -fsanitize=address)
endif()
