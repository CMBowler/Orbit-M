# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

# Find SFML (Requires SFML to be installed)
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

# Include directories for external libraries (JSON for Modern C++)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/loadScenario.cpp
    src/mech.cpp
    src/project.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics)

# Add JSON library (header-only, no linking needed)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Copy resources (like fonts) to the build directory
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/rsc $<TARGET_FILE_DIR:${PROJECT_NAME}>/rsc
)

# Instructions for user
message(STATUS "Build with: cmake . && make")
message(STATUS "Clean with: cmake --build . --target clean")