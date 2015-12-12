find_package(Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

include_directories(${SDL2PP_INCLUDE_DIRS})
link_libraries(${SDL2PP_LIBRARIES})

find_package(CURL REQUIRED)
link_libraries(${CURL_LIBRARIES})
include_directories(${CURL_INCLUDEDIRS})

find_package(Boost COMPONENTS system filesystem REQUIRED)
link_libraries(${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

add_executable(osmview main.cpp mapview.cpp coord.cpp tilecache.cpp
               tilecacheitem.cpp downloader.cpp)
