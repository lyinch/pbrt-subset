//
// Created by Thierry Backes on 2019-06-16.
//

#ifndef PBRT_WHITTED_MAIN_H
#define PBRT_WHITTED_MAIN_H

#include "spectrum.h"
#include <string>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

namespace pbrt{
    // Forward declarations
    template <typename T>
    class Vector2;
    template <typename T>
    class Vector3;
    template <typename T>
    class Point3;
    template <typename T>
    class Point2;
    template <typename T>
    class Normal3;

    typedef RGBSpectrum Spectrum;
    struct CameraSample;
    class FilmTile;
    class VisibilityTester;
    class Scene;
    struct Options {
        Options() {
            cropWindow[0][0] = 0;
            cropWindow[0][1] = 1;
            cropWindow[1][0] = 0;
            cropWindow[1][1] = 1;
        }
        std::string imageFile;
        // x0, x1, y0, y1
        float cropWindow[2][2];
    };

    // constants
    static constexpr float Infinity = std::numeric_limits<float>::infinity();

}

#endif //PBRT_WHITTED_MAIN_H
