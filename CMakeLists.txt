cmake_minimum_required(VERSION 3.1)
project(gltests)
set (CMAKE_CXX_STANDARD 11)



set (sources	glad/src/glad.c
     			gltests-0.cpp)



set ( GLFW_PATH	"C:/libs/glfw-3.2.1")
set ( GLFW_INCLUDE "${GLFW_PATH}/include")
set ( GLFW_LIBS "${GLFW_PATH}/lib/glfw3.lib")

include_directories(${GLFW_INCLUDE})
include_directories(glad/include)


add_executable(gltests-0 ${sources})
target_link_libraries(gltests-0 ${GLFW_LIBS} opengl32.lib)
