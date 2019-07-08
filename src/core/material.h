//
// Created by Thierry Backes on 2019-06-27.
//

#ifndef PBRT_WHITTED_MATERIAL_H
#define PBRT_WHITTED_MATERIAL_H

#include "main.h"
#include "memory.h"

namespace pbrt{
    enum class TransportMode { Radiance, Importance };
    class Material{
    public:
        virtual void ComputeScatteringFunctions(SurfaceInteraction *si,
                                                MemoryArena &arena,
                                                TransportMode mode,
                                                bool allowMultipleLobes) const = 0;
        virtual ~Material();
    };
}
#endif //PBRT_WHITTED_MATERIAL_H
