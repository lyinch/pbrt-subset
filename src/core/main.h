//
// Created by Thierry Backes on 2019-06-16.
//

#ifndef PBRT_WHITTED_MAIN_H
#define PBRT_WHITTED_MAIN_H

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

    class RGBSpectrum;
    typedef RGBSpectrum Spectrum;
    struct CameraSample;
    class FilmTile;
    class VisibilityTester;
    class Scene;
    class Sphere;
    class Shape;
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
    static constexpr float Pi = 3.14159265358979323846;
    static constexpr float InvPi = 0.31830988618379067154;


    template <typename T, typename U, typename V>
    inline T Clamp(T val, U low, V high) {
        if (val < low)
            return low;
        else if (val > high)
            return high;
        else
            return val;
    }

    inline float Radians(float deg) { return (Pi / 180) * deg; }

    inline float Degrees(float rad) { return (180 / Pi) * rad; }
}

#endif //PBRT_WHITTED_MAIN_H
