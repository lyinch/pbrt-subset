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

#include <alloca.h>
#include <iostream>

#ifndef PBRT_L1_CACHE_LINE_SIZE
    #define PBRT_L1_CACHE_LINE_SIZE 64
#endif

namespace pbrt{

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

   // Forward declarations
   class Scene;
   class Integrator;
   class SamplerIntegrator;
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
    class Ray;
    template <typename T>
    class Bounds2;
    template <typename T>
    class Bounds3;
    class Transform;
    struct Interaction;
    class SurfaceInteraction;
    class Shape;
    class Primitive;
    class GeometricPrimitive;
    template <int nSpectrumSamples>
    class CoefficientSpectrum;
    class RGBSpectrum;
    typedef RGBSpectrum Spectrum;
    class Camera;
    struct CameraSample;
    class Sampler;
    class Filter;
    class Film;
    class FilmTile;
    class BxDF;
    class BSDF;
    template <typename T>
    class Texture;
    class Light;
    class VisibilityTester;

    class RNG;
    class MemoryArena;
    struct Matrix4x4;

    struct Options {
        Options() {
            cropWindow[0][0] = 0;
            cropWindow[0][1] = 1;
            cropWindow[1][0] = 0;
            cropWindow[1][1] = 1;
        }
        std::string imageFile;
        int nThreads = 4;
        // x0, x1, y0, y1
        float cropWindow[2][2];
    };

    extern Options PbrtOptions;


    // constants
    static constexpr float Infinity = std::numeric_limits<float>::infinity();
    static constexpr float Pi = 3.14159265358979323846;
    static constexpr float InvPi = 0.31830988618379067154;
    static constexpr float PiOver2 = 1.57079632679489661923;
    static constexpr float PiOver4 = 0.78539816339744830961;
    static constexpr float ShadowEpsilon = 0.0001f;

    inline uint32_t FloatToBits(float f) {
        uint32_t ui;
        memcpy(&ui, &f, sizeof(float));
        return ui;
    }

    inline float BitsToFloat(uint32_t ui) {
        float f;
        memcpy(&f, &ui, sizeof(uint32_t));
        return f;
    }

    inline uint64_t FloatToBits(double f) {
        uint64_t ui;
        memcpy(&ui, &f, sizeof(double));
        return ui;
    }

    inline double BitsToFloat(uint64_t ui) {
        double f;
        memcpy(&f, &ui, sizeof(uint64_t));
        return f;
    }

    inline float NextFloatUp(float v) {
        // Handle infinity and negative zero for _NextFloatUp()_
        if (std::isinf(v) && v > 0.) return v;
        if (v == -0.f) v = 0.f;

        // Advance _v_ to next higher float
        uint32_t ui = FloatToBits(v);
        if (v >= 0)
            ++ui;
        else
            --ui;
        return BitsToFloat(ui);
    }

    inline float NextFloatDown(float v) {
        // Handle infinity and positive zero for _NextFloatDown()_
        if (std::isinf(v) && v < 0.) return v;
        if (v == 0.f) v = -0.f;
        uint32_t ui = FloatToBits(v);
        if (v > 0)
            --ui;
        else
            ++ui;
        return BitsToFloat(ui);
    }

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
