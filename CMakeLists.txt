cmake_minimum_required(VERSION 3.12)
project(pbrt_whitted)

set(CMAKE_CXX_STANDARD 11)

SET ( CORE_SOURCE
        src/core/parser.cpp
        src/core/spectrum.cpp
        src/core/filter.cpp
        src/core/geometry.cpp
        src/core/sampler.cpp
        src/samplers/random.cpp
        src/core/camera.cpp
        src/core/scene.cpp
        src/core/integrator.cpp
        src/core/api.cpp
        src/core/interaction.cpp
        src/core/shape.cpp
        src/core/transform.cpp
        src/core/film.cpp
        src/core/memory.cpp
        src/core/light.cpp
        src/core/primitive.cpp
        src/core/reflection.cpp
        src/core/material.cpp
        src/core/texture.cpp
        )


SET ( CORE_HEADERS
        src/core/api.h
        src/core/parser.h
        src/core/spectrum.h
        src/core/main.h
        src/core/filter.h
        src/core/geometry.h
        src/core/sampler.h
        src/samplers/random.h
        src/core/camera.h
        src/core/scene.h
        src/core/integrator.h
        src/core/interaction.h
        src/core/shape.h
        src/core/transform.h
        src/core/film.h
        src/core/memory.h
        src/core/light.h
        src/core/primitive.h
        src/core/reflection.h
        src/core/material.h
        src/core/texture.h
        )

FILE ( GLOB SOURCE
        src/filters/*.cpp
        src/cameras/*.cpp
        src/integrators/*.cpp
        src/lights/*.cpp
        src/shapes/*.cpp
        src/materials/*.cpp
        src/textures/*.cpp
        )

INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/core)

ADD_LIBRARY(pbrt STATIC
        ${CORE_SOURCE}
        ${CORE_HEADERS}
        ${SOURCE}
        )


SET(ALL_PBRT_LIBS
    pbrt
)


add_executable(pbrt_whitted src/main/main.cpp)
TARGET_LINK_LIBRARIES ( pbrt_whitted ${ALL_PBRT_LIBS} )
