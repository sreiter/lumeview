cmake_minimum_required(VERSION 3.1)

add_subdirectory (slimesh)

set (GL_VERSION_MAJOR "3")
set (GL_VERSION_MINOR "3")

option (GLFW "Creates a glfw based application" ON)
option (JNI "Creates a shared library for use with the Java Native Interface" OFF)
set (RESOURCE_ROOT_PATH ".." CACHE STRING "Path in which resource folders 'meshes' and 'shaders' are located.")

message (STATUS "")
message (STATUS "GLFW:               " ${GLFW} "\t(Create glfw based application. Values: ON / OFF)")
message (STATUS "JNI:                " ${JNI}  "\t(Create a shared library for use with the Java Native Interface. Values: ON / OFF)")
message (STATUS "RESOURCE_ROOT_PATH: " ${RESOURCE_ROOT_PATH}  "\t(Path in which an executable looks for the resource folders 'meshes' and 'shaders'")
message (STATUS "")

add_definitions (-DRESOURCE_ROOT_PATH=\"${RESOURCE_ROOT_PATH}\")

set (sources
    	arc_ball.cpp
    	arc_ball_view.cpp
    	camera.cpp
        file_util.cpp
        shapes.cpp
        renderer.cpp
        view.cpp
        visualization.cpp
    	window_event_listener.cpp

        imgui/imgui_binding.cpp
    )


# set (shaders
#         flat-edge-shading.gs
#         flat-shading.fs
#         flat-tri-shading.gs
#         no-shading.vs
#         smooth-shading.fs
#         smooth-shading.vs)

# set (meshes
#         box.stl
#         bunny.stl
#         sphere.stl
#         pyra.stl
#         quad.stl)


set (GLAD_PATH	"deps/glad/glad-${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}")

set (depSources
		deps/imgui/imgui.cpp
		deps/imgui/imgui_demo.cpp
		deps/imgui/imgui_draw.cpp

		${GLAD_PATH}/src/glad.c
	)


add_definitions (-DUSE_GL_VERSION_MAJOR=${GL_VERSION_MAJOR})
add_definitions (-DUSE_GL_VERSION_MINOR=${GL_VERSION_MINOR})


include_directories (${GLAD_PATH}/include)
include_directories (deps/glm-0.9.8.5)
include_directories (deps)

find_package(OpenGL REQUIRED)


if (MSVC)
    #set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/EHsc")
endif ()

if (GLFW)
    set (glfwSources    main.cpp
                        deps/imgui/imgui_impl_glfw_gl3.cpp
        )

    set (GLFW_PATH	"deps/glfw-3.2.1")

    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW docs" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Build GLFW install" FORCE)

    add_subdirectory(${GLFW_PATH})
    
    get_target_property(glfw_LibDeps glfw INTERFACE_LINK_LIBRARIES)
    if (NOT glfw_LibDeps)
        set (glfw_LibDeps "")
    endif ()

    set ( GLFW_INCLUDE "${GLFW_PATH}/include")
    link_directories ("${CMAKE_BINARY_DIR}")

    include_directories (${GLFW_INCLUDE})

    set (exeName "slimesh")

    add_executable (${exeName} ${glfwSources} ${sources} ${depSources})
    add_dependencies(${exeName} glfw)
    target_link_libraries (${exeName} ${GLFW_LIBS} glfw3 ${glfw_LibDeps} ${OPENGL_LIBRARIES} slimeshlib)
endif ()

if (JNI)
    set (jniSources     jni/jni_event_listener.cpp
        )

    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})

    add_library(vrl_native_opengl_binding SHARED ${jniSources} ${sources} ${depSources})
    target_link_libraries(vrl_native_opengl_binding ${JNI_LIBRARIES} ${OPENGL_LIBRARIES} slimeshlib)
endif ()


# foreach (file IN LISTS shaders)
#     configure_file("../shaders/${file}" "${CMAKE_BINARY_DIR}/src/shaders/${file}" COPYONLY)
# endforeach (file)

# foreach (file IN LISTS meshes)
#     configure_file("../meshes/${file}" "${CMAKE_BINARY_DIR}/src/meshes/${file}" COPYONLY)
# endforeach (file)
