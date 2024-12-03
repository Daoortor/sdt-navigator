cmake_minimum_required(VERSION 3.29)
project(sdtmaps)

add_definitions(-DPROJECT_ROOT_PATH="${CMAKE_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

FILE(GLOB LIB_SOURCES ${CMAKE_SOURCE_DIR}/lib/*.cpp)
include_directories(${CMAKE_SOURCE_DIR}/lib)

FILE(GLOB Models models/src/*.cpp)

add_executable(sdtmaps app/main.cpp core/src/pathfinder.cpp ${Models} ${LIB_SOURCES})
target_link_libraries(sdtmaps
        Qt::Core
)

add_executable(test
        core/src/pathfinder.cpp
        models/src/transportSystem.cpp
        test/main.cpp
        test/test-data/minsk-subway.cpp
        ${LIB_SOURCES})
target_link_libraries(test
        Qt::Core
)