//
// Created by Thierry Backes on 2019-06-18.
//

#include "sampler.h"
#include "sampling.h"
#include "camera.h"


namespace pbrt{
    Sampler::~Sampler() {}

    Sampler::Sampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel){}

    CameraSample Sampler::GetCameraSample(const Point2i &pRaster) {
        CameraSample cs;
        cs.pFilm = (Point2f)pRaster + Get2D();
        return cs;
    }

    void Sampler::StartPixel(Point2i &p) {
        currentPixel = p;
        currentPixelSampleIndex = 0;

    }

    bool Sampler::StartNextSample() {
        return ++currentPixelSampleIndex < samplesPerPixel;
    }

}
