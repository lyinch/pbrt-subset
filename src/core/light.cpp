//
// Created by Thierry Backes on 2019-06-24.
//

#include "light.h"
#include "scene.h"
#include "sampling.h"

namespace pbrt{

    Light::~Light() {

    }

    Light::Light(int flags, const Transform &LightToWorld, int nSamples): flags(flags),
                                                                          nSamples(std::max(1, nSamples)),
                                                                          LightToWorld(LightToWorld),
                                                                          WorldToLight(Inverse(LightToWorld)) {

    }

    Spectrum Light::Le(const RayDifferential &r) const {
        return Spectrum(0.f);
    }

    bool VisibilityTester::Unoccluded(const Scene &scene) const {
        return !scene.IntersectP(p0.SpawnRayTo(p1));

    }
}