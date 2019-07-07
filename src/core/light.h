//
// Created by Thierry Backes on 2019-06-24.
//

#ifndef PBRT_WHITTED_LIGHT_H
#define PBRT_WHITTED_LIGHT_H

#include "main.h"
#include "transform.h"
#include "geometry.h"
#include "interaction.h"
#include "sampler.h"

namespace pbrt{

    enum class LightFlags : int {
        DeltaPosition = 1,
        DeltaDirection = 2,
        Area = 4,
        Infinite = 8
    };

    inline bool IsDeltaLight(int flags) {
        return flags & (int)LightFlags::DeltaPosition ||
               flags & (int)LightFlags::DeltaDirection;
    }

    class Light{
    public:
        virtual ~Light();
        Light(int flags, const Transform &LightToWorld, int nSamples = 1);
        virtual Spectrum Sample_Li(const Interaction &ref, const Point2f &u,
                                   Vector3f *wi, float *pdf,
                                   VisibilityTester *vis) const = 0;
        virtual Spectrum Le(const RayDifferential &r) const;

        virtual Spectrum Power() const = 0;

        const int flags;
        const int nSamples;
    protected:
        const Transform LightToWorld, WorldToLight;
    };

    class VisibilityTester {
    public:
        VisibilityTester() {}
        VisibilityTester(const Interaction &p0, const Interaction &p1) : p0(p0), p1(p1) {}
        const Interaction &P0() const { return p0; }
        const Interaction &P1() const { return p1; }
        bool Unoccluded(const Scene &scene) const;
        Spectrum Tr(const Scene &scene, Sampler &sampler) const;


    private:
        Interaction p0, p1;
    };
}
#endif //PBRT_WHITTED_LIGHT_H
