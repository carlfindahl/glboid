# Data Oriented Boid GL Implementation

project("Boid_GL" VERSION 0.0.1 CXX)

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
               )

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glm)

# GLFW
add_subdirectory("${CMAKE_SOURCE_DIR}/ext/glfw")

# GLM
add_subdirectory("${CMAKE_SOURCE_DIR}/ext/glm")

