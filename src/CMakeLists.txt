cmake_minimum_required(VERSION 3.1)

set (GL_VERSION_MAJOR "3")
set (GL_VERSION_MINOR "3")

option (GLFW "Creates a glfw based application" ON)
option (JNI "Creates a shared library for use with the Java Native Interface" OFF)

message (STATUS "")
message (STATUS "GLFW: " ${GLFW} "\t(Create glfw based application. Values: ON / OFF)")
message (STATUS "JNI:  " ${JNI}  "\t(Create a shared library for use with the Java Native Interface. Values: ON / OFF)")
message (STATUS "")

set (sources
    	arc_ball.cpp
    	arc_ball_view.cpp
    	camera.cpp
        file_util.cpp
        renderer.cpp
        view.cpp
    	window_event_listener.cpp
    )


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
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/EHsc")
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

    set ( GLFW_INCLUDE "${GLFW_PATH}/include")
    link_directories ("${CMAKE_BINARY_DIR}/src/deps/glfw-3.2.1/src/")

    include_directories (${GLFW_INCLUDE})

    set (exeName "gltests-0-gl${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}")

    add_executable (${exeName} ${glfwSources} ${sources} ${depSources})
    add_dependencies(${exeName} glfw)
    target_link_libraries (${exeName} ${GLFW_LIBS} glfw3 ${OPENGL_LIBRARIES})

    file(COPY "../shaders" DESTINATION "${CMAKE_BINARY_DIR}/src")
    file(COPY "../meshes" DESTINATION "${CMAKE_BINARY_DIR}/src")
endif ()

if (JNI)
    set (jniSources     jni/jni_event_listener.cpp
        )

    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})

    add_library(vrl_native_opengl_binding SHARED ${jniSources} ${sources} ${depSources})
    target_link_libraries(vrl_native_opengl_binding ${JNI_LIBRARIES} ${OPENGL_LIBRARIES})
endif ()
