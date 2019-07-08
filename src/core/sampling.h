//
// Created by Thierry Backes on 2019-07-08.
//

#ifndef PBRT_WHITTED_SAMPLING_H
#define PBRT_WHITTED_SAMPLING_H

#include "main.h"
#include "geometry.h"
#include "rng.h"
#include <algorithm>

namespace pbrt{
    Point2f ConcentricSampleDisk(const Point2f &u);

    inline Vector3f CosineSampleHemisphere(const Point2f &u) {
        Point2f d = ConcentricSampleDisk(u);
        float z = std::sqrt(std::max((float)0, 1 - d.x * d.x - d.y * d.y));
        return Vector3f(d.x, d.y, z);
    }

}
#endif //PBRT_WHITTED_SAMPLING_H
