//
// Created by Thierry Backes on 2019-06-18.
//

#ifndef PBRT_WHITTED_BOX_H
#define PBRT_WHITTED_BOX_H

#include <core/filter.h>

namespace pbrt{
    class BoxFilter: public Filter{
    public:
        BoxFilter(const Vector2f &radius): Filter(radius) {}
        float Evaluate(const Point2f &p) const override;
    };
    BoxFilter *CreateBoxFilter();
}


#endif //PBRT_WHITTED_BOX_H
