//
// Created by Thierry Backes on 2019-06-18.
//

#ifndef PBRT_WHITTED_SAMPLER_H
#define PBRT_WHITTED_SAMPLER_H

#include "main.h"
#include "geometry.h"
#include "rng.h"

namespace pbrt {
    class Sampler{
    public:
        virtual ~Sampler();
        Sampler (int64_t samplesPerPixel);
        virtual void StartPixel(const Point2i &p);
        // return sample value for the next or the two next dimensions
        virtual float Get1D() = 0;
        virtual Point2f Get2D() = 0;
        CameraSample GetCameraSample(const Point2i &pRaster);
        virtual std::unique_ptr<Sampler> Clone(int seed) = 0;
        virtual bool StartNextSample();

        const int64_t samplesPerPixel;
    protected:
        Point2i currentPixel;
        int64_t currentPixelSampleIndex;

    private:

    };
}
#endif //PBRT_WHITTED_SAMPLER_H
