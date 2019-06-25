//
// Created by Thierry Backes on 2019-06-17.
//

#ifndef PBRT_WHITTED_FILTER_H
#define PBRT_WHITTED_FILTER_H

#include "geometry.h"

namespace pbrt{
    class Filter {
    public:
        virtual ~Filter();
        Filter(const Vector2f &radius):radius(radius), invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}
        virtual float Evaluate (const Point2f &p) const = 0;

        const Vector2f radius, invRadius;
    };

}
#endif //PBRT_WHITTED_FILTER_H
