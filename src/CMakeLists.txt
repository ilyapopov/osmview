find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

link_libraries(${SDL_LIBRARY} ${SDLIMAGE_LIBRARY})

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -Wno-unused-parameter -Wold-style-cast")
#set(CMAKE_BUILD_TYPE "Release")

add_executable(osmview main.cpp mapview.cpp coord.cpp tilecache.cpp tilecacheitem.cpp timer.cpp tilefetcher.cpp)
