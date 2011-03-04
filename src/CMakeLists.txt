find_package(SDL REQUIRED)
find_package(PNG REQUIRED)

link_libraries(${SDL_LIBRARY} ${PNG_LIBRARY})

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -Wno-unused-parameter")
#set(CMAKE_BUILD_TYPE "Release")

add_executable(osmview main.cpp tile.cpp mapview.cpp coord.cpp tilecache.cpp tilecacheitem.cpp timer.cpp)
