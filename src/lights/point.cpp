//
// Created by Thierry Backes on 2019-06-24.
//

#include "lights/point.h"
#include "scene.h"
#include "sampling.h"

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

    std::shared_ptr<PointLight> CreatePointLight(const Transform &light2world) {
        float rgb[3] = {.5,.5,.5};
        Spectrum I = Spectrum::FromRGB(rgb);
        Spectrum sc = Spectrum(1.0);
        Point3f P = Point3f(0,0,0);
        Transform l2w = Translate(Vector3f(P.x, P.y, P.z)) * light2world;
        return std::make_shared<PointLight>(l2w, I * sc);
    }
}