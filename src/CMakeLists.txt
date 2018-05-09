cmake_minimum_required(VERSION 3.1)
project(gltests)
set (CMAKE_CXX_STANDARD 11)

set (GL_VERSION_MAJOR "3")
set (GL_VERSION_MINOR "3")


set (sources
     	main.cpp
    	camera/arc_ball.cpp
    )



set (GLAD_PATH	"deps/glad/glad-${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}")

set (depSources
		deps/imgui/imgui.cpp
		deps/imgui/imgui_demo.cpp
		deps/imgui/imgui_draw.cpp
		deps/imgui/imgui_impl_glfw_gl3.cpp

		${GLAD_PATH}/src/glad.c
	)


add_definitions (-DUSE_GL_VERSION_MAJOR=${GL_VERSION_MAJOR})
add_definitions (-DUSE_GL_VERSION_MINOR=${GL_VERSION_MINOR})


set ( GLFW_PATH	"deps/glfw-3.2.1")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW docs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Build GLFW install" FORCE)

add_subdirectory(${GLFW_PATH})


set ( GLFW_INCLUDE "${GLFW_PATH}/include")
set ( GLFW_LIBS "${CMAKE_BINARY_DIR}/deps/glfw-3.2.1/src/glfw3.lib")

include_directories (${GLFW_INCLUDE})
include_directories (${GLAD_PATH}/include)
include_directories (deps/glm-0.9.8.5)
include_directories (deps)

set (exeName "gltests-0-gl${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}")

add_executable (${exeName} ${sources} ${depSources})
add_dependencies(${exeName} glfw)
target_link_libraries (${exeName} ${GLFW_LIBS} opengl32.lib)
