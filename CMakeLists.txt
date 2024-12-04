cmake_minimum_required(VERSION 3.29)
project(sdtmaps)

add_definitions(-DPROJECT_ROOT_PATH="${CMAKE_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt library
find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

# Doctest library
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${CMAKE_SOURCE_DIR}/lib/doctest/doctest)

# Rapidjson library
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${CMAKE_SOURCE_DIR}/lib/rapidjson/include)

# Other libraries
include_directories(${CMAKE_SOURCE_DIR}/lib)

FILE(GLOB Models models/src/*.cpp)

# Main programm
add_executable(sdtmaps app/main.cpp core/src/pathfinder.cpp ${Models})
target_link_libraries(sdtmaps
        Qt::Core
        rapidjson
)

# Tests
add_executable(test
        core/src/pathfinder.cpp
        models/src/transportSystem.cpp
        test/main.cpp
        test/test-data/minsk-subway.cpp)
target_link_libraries(test
        Qt::Core
        rapidjson
        doctest
)