//
// Created by Thierry Backes on 2019-06-19.
//

#include "integrators/whitted.h"
#include "interaction.h"
#include "camera.h"
#include "film.h"
namespace pbrt {
    Spectrum WhittedIntegrator::Li(const RayDifferential &ray, const pbrt::Scene &scene, pbrt::Sampler &sampler,
                                MemoryArena &arena, int depth) const {

        Spectrum L(0.);
        SurfaceInteraction isect;
        if (!scene.Intersect(ray, &isect)) {
            for (const auto &light : scene.lights) L += light->Le(ray);
            return L;
        }

        const Normal3f &n = isect.shading.n;
        Vector3f wo = isect.wo;

        isect.ComputeScatteringFunctions(ray, arena);
        if (!isect.bsdf)
            return Li(isect.SpawnRay(ray.d), scene, sampler, arena, depth);

        L += isect.Le(wo);

        for (const auto &light : scene.lights) {
            Vector3f wi;
            float pdf;
            VisibilityTester visibility; /* ask a visibility tester function from the light source*/
            Spectrum Li =
                    light->Sample_Li(isect, sampler.Get2D(), &wi, &pdf,
                                     &visibility); /* returns radiance of light source */
            if (Li.IsBlack() || pdf == 0) continue;
            Spectrum f = isect.bsdf->f(wo, wi);
            if (!f.IsBlack() && visibility.Unoccluded(scene))
                L += f * Li * AbsDot(wi, n) / pdf;
        }
        if (depth + 1 < maxDepth) {
            // Trace rays for specular reflection and refraction
            L += SpecularReflect(ray, isect, scene, sampler, arena, depth);
            L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
        }

        return L;
    }

    WhittedIntegrator * CreateWhittedIntegrator(std::shared_ptr<pbrt::Sampler> sampler, std::shared_ptr<const pbrt::Camera> camera) {
            int maxDepth = 5;
            int np;
            const int *pb = &np;
            Bounds2i pixelBounds = camera->film->GetSampleBounds();
            if (pb) {
                if (np != 4){}
                else {
                    pixelBounds = Intersect(pixelBounds,
                                            Bounds2i{{pb[0], pb[2]}, {pb[1], pb[3]}});
                    if (pixelBounds.Area() == 0){}
                }
            }
            return new WhittedIntegrator(maxDepth, camera, sampler, pixelBounds);
        }
}
