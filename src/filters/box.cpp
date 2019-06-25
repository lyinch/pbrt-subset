//
// Created by Thierry Backes on 2019-06-18.
//

#include "box.h"
namespace pbrt {
    float BoxFilter::Evaluate(const Point2f &p) const {
        return 1.;
    }

    BoxFilter *CreateBoxFilter() {
        return new BoxFilter(Vector2f(0.5f,0.5f));
    }

}
