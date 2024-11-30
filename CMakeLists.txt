cmake_minimum_required(VERSION 3.29)
project(sdtmaps)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

FILE(GLOB Models models/src/*.cpp)

add_executable(sdtmaps app/main.cpp core/src/pathfinder.cpp ${Models})
target_link_libraries(sdtmaps
        Qt::Core
)
