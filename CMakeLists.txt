project(osmview C CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(SDL2PP_WITH_IMAGE ON) # if you need SDL_image support
#set(SDL2PP_WITH_MIXER ON) # if you need SDL_mixer support
set(SDL2PP_WITH_TTF ON) # if you need SDL_ttf support
add_subdirectory(deps/libSDL2pp)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_executable(osmview
               src/main.cpp src/mapview.cpp src/coord.cpp src/tilecache.cpp
               src/tilecacheitem.cpp src/downloader.cpp)

target_link_libraries(osmview PRIVATE ${CMAKE_THREAD_LIBS_INIT}
    ${SDL2PP_LIBRARIES} ${SDLMAIN_LIBRARY} ${CURL_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(osmview SYSTEM PRIVATE ${SDL2PP_INCLUDE_DIRS}
    ${CURL_INCLUDEDIRS} ${Boost_INCLUDE_DIRS})
target_compile_features(osmview PRIVATE cxx_auto_type cxx_range_for cxx_nullptr
    cxx_noexcept cxx_constexpr cxx_lambdas cxx_right_angle_brackets
    cxx_strong_enums cxx_uniform_initialization)
target_compile_options(osmview PRIVATE -Wall -Wextra -Wpedantic)
