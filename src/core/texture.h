//
// Created by Thierry Backes on 2019-06-27.
//

#ifndef PBRT_WHITTED_TEXTURE_H
#define PBRT_WHITTED_TEXTURE_H

#include "main.h"
#include "spectrum.h"
#include "geometry.h"
#include "transform.h"
#include "memory.h"

namespace pbrt{
    template <typename T>
    class Texture {
    public:
        // Texture Interface
        virtual T Evaluate(const SurfaceInteraction &) const = 0;
        virtual ~Texture() {}
    };
}
#endif //PBRT_WHITTED_TEXTURE_H
