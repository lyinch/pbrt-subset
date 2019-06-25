//
// Created by Thierry Backes on 2019-06-24.
//

#include "point.h"
namespace pbrt{

    Spectrum PointLight::Sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wi, float *pdf,
                                   VisibilityTester *vis) const {
        *wi = Normalize(pLight - ref.p);
        *pdf = 1.f;
        *vis =VisibilityTester(ref, Interaction(pLight));
        return I / DistanceSquared(pLight, ref.p);
    }

    Spectrum PointLight::Power() const {
        return 4 * Pi * I;
    }
}