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
add_executable(sdtmaps ${Models} core/src/pathfinder.cpp app/main.cpp)
target_link_libraries(sdtmaps
        Qt::Core
        rapidjson
)

# Tests
add_executable(sdtmaps_test
        ${Models}
        core/src/pathfinder.cpp
        test/main.cpp
        test/test-data/minsk-subway.cpp)
target_link_libraries(sdtmaps_test
        Qt::Core
        rapidjson
        doctest
)

enable_testing()
add_test(NAME sdtmaps_tests COMMAND test)
