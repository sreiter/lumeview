#todo: rename to slimesh once it is in its own repository
project (slimesh_core)
cmake_minimum_required(VERSION 3.1)

set (sources
        subset_info_annex.cpp
        file_io.cpp
        grob.cpp
        mesh.cpp
        normals.cpp
        topology.cpp
    )

include_directories (deps/rapidxml-1.13)
include_directories (deps)

add_library (slimeshlib ${sources})
