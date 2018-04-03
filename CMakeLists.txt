# Data Oriented Boid GL Implementation
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)


project("Boid_GL" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} "")

# Set include dirs
target_include_directories(${PROJECT_NAME}
                           PRIVATE
                           ${CMAKE_SOURCE_DIR}/include
                           )

# Add source files
target_sources(${PROJECT_NAME}
               PRIVATE
               ${CMAKE_SOURCE_DIR}/src/main.cpp
               ${CMAKE_SOURCE_DIR}/src/gl_core4_5.cpp
               ${CMAKE_SOURCE_DIR}/src/flock.h
               ${CMAKE_SOURCE_DIR}/src/flock.cpp
               ${CMAKE_SOURCE_DIR}/src/detail.cpp
               ${CMAKE_SOURCE_DIR}/src/detail.h
               ${CMAKE_SOURCE_DIR}/include/gl_core4_5.hpp
               )

# GLFW
find_package(glfw3 3.2 REQUIRED)

# GLM
find_package(glm REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# Link Dependencies
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glm)
