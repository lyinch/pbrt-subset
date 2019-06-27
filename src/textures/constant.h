//
// Created by Thierry Backes on 2019-06-27.
//

#ifndef PBRT_WHITTED_CONSTANT_H
#define PBRT_WHITTED_CONSTANT_H

#include "texture.h"
namespace pbrt{
    template <typename T>
    class ConstantTexture : public Texture<T> {
        ConstantTexture(const T &value) : value(value) {}
        T Evaluate(const SurfaceInteraction &) const { return value; }

    private:
        T value;
    };
}
#endif //PBRT_WHITTED_CONSTANT_H
