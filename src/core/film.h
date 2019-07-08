//
// Created by Thierry Backes on 2019-06-20.
//

#ifndef PBRT_WHITTED_FILM_H
#define PBRT_WHITTED_FILM_H

#include "main.h"
#include "geometry.h"
#include "filter.h"
#include "spectrum.h"
#include "parallel.h"

namespace pbrt{
    struct FilmTilePixel {
        Spectrum contribSum = 0.f;
        float filterWeightSum = 0.f;
    };

    class Film{
    public:
        Film(const Point2i &resolution,const Bounds2f &cropWindow,
             std::unique_ptr<Filter> filter,
             const std::string &filename, float scale);
        Bounds2i GetSampleBounds() const;
        std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i &sampleBounds);
        void MergeFilmTile(std::unique_ptr<FilmTile> tile);

        const Point2i fullResolution;
        std::unique_ptr<Filter> filter;
        const std::string filename;
        Bounds2i croppedPixelBounds;

    private:
        const float scale;
        struct Pixel {
            Pixel() { xyz[0] = xyz[1] = xyz[2] = filterWeightSum = 0; }
            float xyz[3];
            float filterWeightSum;
            AtomicFloat splatXYZ[3];
            float pad;
        };

        Pixel &GetPixel(const Point2i &p) {
            assert(InsideExclusive(p, croppedPixelBounds));
            int width = croppedPixelBounds.pMax.x - croppedPixelBounds.pMin.x;
            int offset = (p.x - croppedPixelBounds.pMin.x) +
                         (p.y - croppedPixelBounds.pMin.y) * width;
            return pixels[offset];
        }

        std::unique_ptr<Pixel[]> pixels;
        static constexpr int filterTableWidth = 16;
        float filterTable[filterTableWidth * filterTableWidth];
        std::mutex mutex;
    };

    class FilmTile {
    public:
        FilmTile(const Bounds2i &pixelBounds, const Vector2f &filterRadius,
                 const float *filterTable, int filterTableSize)
                : pixelBounds(pixelBounds),
                  filterRadius(filterRadius),
                  invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
                  filterTable(filterTable),
                  filterTableSize(filterTableSize) {
            pixels = std::vector<FilmTilePixel>(std::max(0, pixelBounds.Area()));
        }

        void AddSample(const Point2f &pFilm, Spectrum L,
                       float sampleWeight = 1.) {

            Point2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
            Point2i p0 = (Point2i)Ceil(pFilmDiscrete - filterRadius);
            Point2i p1 =
                    (Point2i)Floor(pFilmDiscrete + filterRadius) + Point2i(1, 1);
            p0 = Max(p0, pixelBounds.pMin);
            p1 = Min(p1, pixelBounds.pMax);

            int *ifx = ALLOCA(int, p1.x - p0.x);
            for (int x = p0.x; x < p1.x; ++x) {
                float fx = std::abs((x - pFilmDiscrete.x) * invFilterRadius.x *
                                    filterTableSize);
                ifx[x - p0.x] = std::min((int)std::floor(fx), filterTableSize - 1);
            }
            int *ify = ALLOCA(int, p1.y - p0.y);
            for (int y = p0.y; y < p1.y; ++y) {
                float fy = std::abs((y - pFilmDiscrete.y) * invFilterRadius.y *
                                    filterTableSize);
                ify[y - p0.y] = std::min((int)std::floor(fy), filterTableSize - 1);
            }

            for (int y = p0.y; y < p1.y; ++y) {
                for (int x = p0.x; x < p1.x; ++x) {
                    // Evaluate filter value at $(x,y)$ pixel
                    int offset = ify[y - p0.y] * filterTableSize + ifx[x - p0.x];
                    float filterWeight = filterTable[offset];

                    // Update pixel values with filtered sample contribution
                    FilmTilePixel &pixel = GetPixel(Point2i(x, y));
                    pixel.contribSum += L * sampleWeight * filterWeight;
                    pixel.filterWeightSum += filterWeight;
                }
            }
        }

        FilmTilePixel &GetPixel(const Point2i &p) {
            int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
            int offset =
                    (p.x - pixelBounds.pMin.x) + (p.y - pixelBounds.pMin.y) * width;
            return pixels[offset];
        }

        const FilmTilePixel &GetPixel(const Point2i &p) const {
            int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
            int offset =
                    (p.x - pixelBounds.pMin.x) + (p.y - pixelBounds.pMin.y) * width;
            return pixels[offset];
        }

        Bounds2i GetPixelBounds() const { return pixelBounds; }


    private:
        const Bounds2i pixelBounds;
        const Vector2f filterRadius, invFilterRadius;
        const float *filterTable;
        const int filterTableSize;
        std::vector<FilmTilePixel> pixels;
        friend class Film;
    };

    Film *CreateFilm(std::unique_ptr<Filter> filter);

}
#endif //PBRT_WHITTED_FILM_H
