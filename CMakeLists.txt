cmake_minimum_required(VERSION 3.16)
project(Lumine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LUMINE_BUILD_EXAMPLES "Build Example CLI app" ON)
option(LUMINE_BUILD_TESTS "Build tests" OFF)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)

add_library(lumine_core
    src/image.cpp
    src/kernel.cpp
    src/convolver.cpp
    src/preprocessing.cpp
)

# target_include_directories(lumine_core PUBLIC ${PROJECT_SOURCE_DIR}/incude)
include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(lumine_core PUBLIC stb)

if (LUMINE_BUILD_EXAMPLES)
    add_executable(lumine src/main.cpp)
    target_link_libraries(lumine PRIVATE lumine_core)
endif()
