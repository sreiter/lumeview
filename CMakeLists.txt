cmake_minimum_required(VERSION 3.1)
project(gltests)
set (CMAKE_CXX_STANDARD 11)

set (GL_VERSION_MAJOR "3")
set (GL_VERSION_MINOR "3")
set (GLAD_PATH	"glad/glad-${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}")


set (sources	${GLAD_PATH}/src/glad.c

     			camera/arc_ball.cpp
     			gltests-0.cpp

     			imgui/imgui.cpp
     			imgui/imgui_demo.cpp
     			imgui/imgui_draw.cpp
     			imgui/imgui_impl_glfw_gl3.cpp
     			)


add_definitions (-DUSE_GL_VERSION_MAJOR=${GL_VERSION_MAJOR})
add_definitions (-DUSE_GL_VERSION_MINOR=${GL_VERSION_MINOR})


set ( GLFW_PATH	"C:/libs/glfw-3.2.1")
set ( GLFW_INCLUDE "${GLFW_PATH}/include")
set ( GLFW_LIBS "${GLFW_PATH}/lib/glfw3.lib")

include_directories (${GLFW_INCLUDE})
include_directories (${GLAD_PATH}/include)
include_directories (glm/glm-0.9.8.5)

set (exeName "gltests-0-gl${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}")

add_executable (${exeName} ${sources})
target_link_libraries (${exeName} ${GLFW_LIBS} opengl32.lib)
