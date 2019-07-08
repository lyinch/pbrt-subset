//
// Created by Thierry Backes on 2019-06-19.
//

#include "integrator.h"
#include "scene.h"
#include "interaction.h"
#include "parallel.h"
#include "film.h"
#include "sampler.h"

#include "camera.h"

namespace pbrt{
    Integrator::~Integrator() {}

    void SamplerIntegrator::Render(const Scene &scene) {
        Bounds2i sampleBounds = camera->film->GetSampleBounds();
        Vector2i sampleExtent = sampleBounds.Diagonal();
        const int tileSize = 16;
        Point2i nTiles((sampleExtent.x + tileSize - 1) / tileSize,
                       (sampleExtent.y + tileSize - 1) / tileSize);

        {
            ParallelFor2D([&](Point2i tile) {
                // Allocate _MemoryArena_ for tile
                MemoryArena arena;

                // Get sampler instance for tile
                int seed = tile.y * nTiles.x + tile.x;
                std::unique_ptr<Sampler> tileSampler = sampler->Clone(seed);

                // Compute sample bounds for tile
                int x0 = sampleBounds.pMin.x + tile.x * tileSize;
                int x1 = std::min(x0 + tileSize, sampleBounds.pMax.x);
                int y0 = sampleBounds.pMin.y + tile.y * tileSize;
                int y1 = std::min(y0 + tileSize, sampleBounds.pMax.y);
                Bounds2i tileBounds(Point2i(x0, y0), Point2i(x1, y1));

                // Get _FilmTile_ for tile
                std::unique_ptr<FilmTile> filmTile =
                        camera->film->GetFilmTile(tileBounds);

                for (Point2i pixel : tileBounds) {
                    {
                        tileSampler->StartPixel(pixel);
                    }

                    // Do this check after the StartPixel() call; this keeps
                    // the usage of RNG values from (most) Samplers that use
                    // RNGs consistent, which improves reproducability /
                    // debugging.
                    if (!InsideExclusive(pixel, pixelBounds))
                        continue;

                    do {
                        // Initialize _CameraSample_ for current sample
                        CameraSample cameraSample =
                                tileSampler->GetCameraSample(pixel);

                        // Generate camera ray for current sample
                        RayDifferential ray;
                        float rayWeight =
                                camera->GenerateRayDifferential(cameraSample, &ray);
                        ray.ScaleDifferentials(
                                1 / std::sqrt((float) tileSampler->samplesPerPixel));

                        // Evaluate radiance along camera ray
                        Spectrum L(0.f);
                        if (rayWeight > 0) L = Li(ray, scene, *tileSampler, arena);

                        // Issue warning if unexpected radiance value returned
                        if (L.HasNaNs()) {
                            L = Spectrum(0.f);
                        } else if (L.y() < -1e-5) {
                            L = Spectrum(0.f);
                        } else if (std::isinf(L.y())) {
                            L = Spectrum(0.f);
                        }


                        // Add camera ray's contribution to image
                        filmTile->AddSample(cameraSample.pFilm, L, rayWeight);

                        // Free _MemoryArena_ memory from computing image sample
                        // value
                        arena.Reset();
                    } while (tileSampler->StartNextSample());
                    }
                camera->film->MergeFilmTile(std::move(filmTile));
            }, nTiles);
        }
    }

    Spectrum
    SamplerIntegrator::SpecularReflect(const RayDifferential &ray, const SurfaceInteraction &isect, const Scene &scene,
                                       Sampler &sampler, MemoryArena &arena, int depth) const {
        // Compute specular reflection direction _wi_ and BSDF value
        Vector3f wo = isect.wo, wi;
        float pdf;
        BxDFType type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);
        Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, type);
        // Return contribution of specular reflection
        const Normal3f &ns = isect.shading.n;
        if (pdf > 0.f && !f.IsBlack() && AbsDot(wi, ns) != 0.f) {
            // Compute ray differential _rd_ for specular reflection
            RayDifferential rd = isect.SpawnRay(wi);
            if (ray.hasDifferentials) {
                rd.hasDifferentials = true;
                rd.rxOrigin = isect.p + isect.dpdx;
                rd.ryOrigin = isect.p + isect.dpdy;
                // Compute differential reflected directions
                Normal3f dndx = isect.shading.dndu * isect.dudx +
                                isect.shading.dndv * isect.dvdx;
                Normal3f dndy = isect.shading.dndu * isect.dudy +
                                isect.shading.dndv * isect.dvdy;
                Vector3f dwodx = -ray.rxDirection - wo,
                        dwody = -ray.ryDirection - wo;
                float dDNdx = Dot(dwodx, ns) + Dot(wo, dndx);
                float dDNdy = Dot(dwody, ns) + Dot(wo, dndy);
                rd.rxDirection =
                        wi - dwodx + 2.f * Vector3f(Dot(wo, ns) * dndx + dDNdx * ns);
                rd.ryDirection =
                        wi - dwody + 2.f * Vector3f(Dot(wo, ns) * dndy + dDNdy * ns);
            }
            return f * Li(rd, scene, sampler, arena, depth + 1) * AbsDot(wi, ns) /
                   pdf;
        } else
            return Spectrum(0.f);
    }

    Spectrum
    SamplerIntegrator::SpecularTransmit(const RayDifferential &ray, const SurfaceInteraction &isect, const Scene &scene,
                                        Sampler &sampler, MemoryArena &arena, int depth) const {
        Vector3f wo = isect.wo, wi;
        float pdf;
        const Point3f &p = isect.p;
        const Normal3f &ns = isect.shading.n;
        const BSDF &bsdf = *isect.bsdf;
        Spectrum f = bsdf.Sample_f(wo, &wi, sampler.Get2D(), &pdf,
                                   BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));
        Spectrum L = Spectrum(0.f);
        if (pdf > 0.f && !f.IsBlack() && AbsDot(wi, ns) != 0.f) {
            // Compute ray differential _rd_ for specular transmission
            RayDifferential rd = isect.SpawnRay(wi);
            if (ray.hasDifferentials) {
                rd.hasDifferentials = true;
                rd.rxOrigin = p + isect.dpdx;
                rd.ryOrigin = p + isect.dpdy;

                float eta = bsdf.eta;
                Vector3f w = -wo;
                if (Dot(wo, ns) < 0) eta = 1.f / eta;

                Normal3f dndx = isect.shading.dndu * isect.dudx +
                                isect.shading.dndv * isect.dvdx;
                Normal3f dndy = isect.shading.dndu * isect.dudy +
                                isect.shading.dndv * isect.dvdy;

                Vector3f dwodx = -ray.rxDirection - wo,
                        dwody = -ray.ryDirection - wo;
                float dDNdx = Dot(dwodx, ns) + Dot(wo, dndx);
                float dDNdy = Dot(dwody, ns) + Dot(wo, dndy);

                float mu = eta * Dot(w, ns) - Dot(wi, ns);
                float dmudx =
                        (eta - (eta * eta * Dot(w, ns)) / Dot(wi, ns)) * dDNdx;
                float dmudy =
                        (eta - (eta * eta * Dot(w, ns)) / Dot(wi, ns)) * dDNdy;

                rd.rxDirection =
                        wi + eta * dwodx - Vector3f(mu * dndx + dmudx * ns);
                rd.ryDirection =
                        wi + eta * dwody - Vector3f(mu * dndy + dmudy * ns);
            }
            L = f * Li(rd, scene, sampler, arena, depth + 1) * AbsDot(wi, ns) / pdf;
        }
        return L;
    }

}
