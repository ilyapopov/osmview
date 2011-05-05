find_package(SDL REQUIRED)
link_libraries(${SDL_LIBRARY})
include_directories(${SDL_INCLUDEDIR})

find_package(SDL_image REQUIRED)
link_libraries(${SDLIMAGE_LIBRARY})
include_directories(${SDLIMAGE_INCLUDEDIR})

find_package(CURL REQUIRED)
link_libraries(${CURL_LIBRARIES})
include_directories(${CURL_INCLUDEDIRS})

find_package(Boost COMPONENTS filesystem REQUIRED)
link_libraries(${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x -Wall -Wextra -Wno-unused-parameter -Wold-style-cast")
#set(CMAKE_BUILD_TYPE "Release")

add_executable(osmview main.cpp mapview.cpp coord.cpp tilecache.cpp tilecacheitem.cpp)
