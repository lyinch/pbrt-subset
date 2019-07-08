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

}
