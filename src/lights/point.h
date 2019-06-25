//
// Created by Thierry Backes on 2019-06-24.
//

#ifndef PBRT_WHITTED_POINT_H
#define PBRT_WHITTED_POINT_H

#include <core/light.h>

namespace pbrt{
    class PointLight : public Light {
    public:
        PointLight(const Transform &LightToWorld,
                   const Spectrum &I)
                : Light((int)LightFlags::DeltaPosition, LightToWorld),
                  pLight(LightToWorld(Point3f(0, 0, 0))),
                  I(I) {}
        Spectrum Sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wi, float *pdf,
                           VisibilityTester *vis) const override;

        Spectrum Power() const override;

    private:
        const Point3f pLight;
        const Spectrum I;

    };
}
#endif //PBRT_WHITTED_POINT_H
