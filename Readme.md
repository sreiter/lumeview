# lumeview

## Introduction
**lumeview** (**L**ightweight **U**nstructured **ME**shes **VIEW**er) is a *BSD* licensed C++ application for the visualization of unstructured hybrid meshes (or unstructured hybrid grids).

Supported mesh-element types are: *Vertices, Edges, Triangles, Quadrilaterals, Tetrahedra, Pyramids, Prisms, Hexahedra*.

**lumeview** is based on *OpenGL* and uses external libraries like *glfw*, and *imgui* for rendering and user interaction. The underlying mesh-library is *lume* (https://github.com/sreiter/lume).

**lumeview** comes as a self contained repository with all dependencies included. Building **lumeview** should thus be straight forward (see below for more details).

**PLEASE NOTE:** **lumeview** is still in development and not yet feature complete.

For a more versatile mesh viewer and editor, albeit more heavyweight and a bit slower, you may also want to consider **ProMesh**: www.promesh3d.com or https://github.com/sreiter/ProMesh

## Building lumeview
**lumeview** requires a *C++ 11* compatible compiler. In the **lumeview** root folder, create a 'build' directory. From that 'build' directory, execute the following commands to build and execute **lumeview** (example for unix-like environments):

    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j
    ./lumeview

**lumeview** works with the *Visual Studio Compilers* on *Windows*, too. Instead of `make -j` you may open the generated project-files with *Visual Studio* and compile them there (or use nmake/jom).

## Building lumeview for Java through JNI
**lumeview** can also be used as a mesh viewer inside *Java* applications through the *Java Native Interface*. To this end, **lumeview** has to be built with the *JNI* option enabled:

    cmake -DJNI=ON ..
    make -j

or similar on non-unix systems. A sample *Java* application is provided with *lumeview* in `lumeview/java_binding_sample`. Once **lumeview** has been built with *-DJNI=ON*, you may run the following command from inside `lumeview/java_binding_sample` to start a *Java* application based on *lumeview*:

    gradlew run

## License
**lume** is licensed under a *2-clause BSD* license:

    Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>
    Copyright (C) 2018 G-CSC, Goethe University Frankfurt
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Please note that **lumeview** is deployed with the open source libraries **glfw** (zlib/libpng license), **rapidxml**, **glm**, **imgui** (MIT license), **stl_reader**, and **lume** (BSD license).
