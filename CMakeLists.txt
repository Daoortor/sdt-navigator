cmake_minimum_required(VERSION 3.29)
project(sdt-navigator)

add_definitions(-DPROJECT_ROOT_PATH="${CMAKE_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set compiler optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native -mmmx -msse -msse2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(WARNING "Optimizations for MSVC are not fully configured")
endif()

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
FILE(GLOB Core core/src/*.cpp)
FILE(GLOB Test test/)

# Main program
add_executable(sdt-navigator ${Models} ${Core} app/main.cpp)
target_link_libraries(sdt-navigator
        Qt::Core
        rapidjson
)

# Tests
add_executable(sdt-navigator_test
        ${Models}
        ${Core}
        test/main.cpp
        test/util.cpp
        test/test-data/minsk-subway.cpp)
target_link_libraries(sdt-navigator_test
        Qt::Core
        rapidjson
        doctest
)

# CTest
enable_testing()
add_test(NAME test COMMAND sdt-navigator_test)
