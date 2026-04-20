cmake_minimum_required(VERSION 3.24)
project(opengl_project)

set(CMAKE_CXX_STANDARD 17)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(OpenGL REQUIRED)

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

set(LIBS glfw3 opengl32)

set(LIBS ${LIBS} GLAD)

include_directories(${CMAKE_SOURCE_DIR}/includes)

add_library(GLAD STATIC glad.c)
target_include_directories(GLAD PUBLIC includes/)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(opengl_project ${SOURCES})

target_include_directories(opengl_project PRIVATE
    src/
    includes/
)
target_link_libraries(opengl_project
    OpenGL::GL
    glfw
    GLAD
    dl
)
