//
// Created by Thierry Backes on 2019-06-19.
//

#include "interaction.h"

namespace pbrt {
    SurfaceInteraction::SurfaceInteraction(const pbrt::Point3f &p, const pbrt::Vector3f &pError,
                                           const pbrt::Point2f &uv, const pbrt::Vector3f &wo,
                                           const pbrt::Vector3f &dpdu, const pbrt::Vector3f &dpdv,
                                           const pbrt::Normal3f &dndu, const pbrt::Normal3f &dndv,
                                           const pbrt::Shape *sh, int faceIndex):

    Interaction(p, Normal3f(Normalize(Cross(dpdu, dpdv))), pError, wo, nullptr),
    uv(uv),
    dpdu(dpdu),
    dpdv(dpdv),
    dndu(dndu),
    dndv(dndv),
    shape(shape) {
        shading.n = n;
        shading.dpdu = dpdu;
        shading.dpdv = dpdv;
        shading.dndu = dndu;
        shading.dndv = dndv;

    }

    Spectrum SurfaceInteraction::Le(const Vector3f &w) const {
        const AreaLight *area = primitive->GetAreaLight();
        return area ? area->L(*this, w) : Spectrum(0.f);
    }

    void SurfaceInteraction::ComputeScatteringFunctions(const RayDifferential &ray, MemoryArena &arena,
                                                        bool allowMultipleLobes, TransportMode mode) {
        ComputeDifferentials(ray);
        primitive->ComputeScatteringFunctions(this, arena, mode,
                                              allowMultipleLobes);

    }
}
