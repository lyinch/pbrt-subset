//
// Created by Thierry Backes on 2019-06-19.
//

#include "interaction.h"
#include "transform.h"
#include "primitive.h"
#include "shape.h"
#include "light.h"

namespace pbrt {
    SurfaceInteraction::SurfaceInteraction(const pbrt::Point3f &p,
                                           const pbrt::Point2f &uv, const pbrt::Vector3f &wo,
                                           const pbrt::Vector3f &dpdu, const pbrt::Vector3f &dpdv,
                                           const pbrt::Normal3f &dndu, const pbrt::Normal3f &dndv,
                                           const pbrt::Shape *sh, int faceIndex):

    Interaction(p, Normal3f(Normalize(Cross(dpdu, dpdv))), wo),
    uv(uv),
    dpdu(dpdu),
    dpdv(dpdv),
    dndu(dndu),
    dndv(dndv),
    shape(sh) {
        shading.n = n;
        shading.dpdu = dpdu;
        shading.dpdv = dpdv;
        shading.dndu = dndu;
        shading.dndv = dndv;

    }

    Spectrum SurfaceInteraction::Le(const Vector3f &w) const {
        //const AreaLight *area = primitive->GetAreaLight();
        //return area ? area->L(*this, w) : Spectrum(0.f);
        return Spectrum(0.f);
    }

    void SurfaceInteraction::ComputeScatteringFunctions(const RayDifferential &ray, MemoryArena &arena,
                                                        bool allowMultipleLobes, TransportMode mode) {
        ComputeDifferentials(ray);
        primitive->ComputeScatteringFunctions(this, arena, mode,
                                              allowMultipleLobes);

    }

    void SurfaceInteraction::ComputeDifferentials(const RayDifferential &ray) const {
        if (ray.hasDifferentials) {
            // Compute auxiliary intersection points with plane
            float d = Dot(n, Vector3f(p.x, p.y, p.z));
            float tx =
                    -(Dot(n, Vector3f(ray.rxOrigin)) - d) / Dot(n, ray.rxDirection);
            if (std::isinf(tx) || std::isnan(tx)) goto fail;
            Point3f px = ray.rxOrigin + tx * ray.rxDirection;
            float ty =
                    -(Dot(n, Vector3f(ray.ryOrigin)) - d) / Dot(n, ray.ryDirection);
            if (std::isinf(ty) || std::isnan(ty)) goto fail;
            Point3f py = ray.ryOrigin + ty * ray.ryDirection;

            dpdx = px - p;
            dpdy = py - p;

            // Choose two dimensions to use for ray offset computation
            int dim[2];
            if (std::abs(n.x) > std::abs(n.y) && std::abs(n.x) > std::abs(n.z)) {
                dim[0] = 1;
                dim[1] = 2;
            } else if (std::abs(n.y) > std::abs(n.z)) {
                dim[0] = 0;
                dim[1] = 2;
            } else {
                dim[0] = 0;
                dim[1] = 1;
            }

            // Initialize _A_, _Bx_, and _By_ matrices for offset computation
            float A[2][2] = {{dpdu[dim[0]], dpdv[dim[0]]},
                             {dpdu[dim[1]], dpdv[dim[1]]}};
            flaot Bx[2] = {px[dim[0]] - p[dim[0]], px[dim[1]] - p[dim[1]]};
            float By[2] = {py[dim[0]] - p[dim[0]], py[dim[1]] - p[dim[1]]};
            if (!SolveLinearSystem2x2(A, Bx, &dudx, &dvdx)) dudx = dvdx = 0;
            if (!SolveLinearSystem2x2(A, By, &dudy, &dvdy)) dudy = dvdy = 0;

        }else {
        fail:
            dudx = dvdx = 0;
            dudy = dvdy = 0;
            dpdx = dpdy = Vector3f(0, 0, 0);
        }
    }
}
