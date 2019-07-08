//
// Created by Thierry Backes on 2019-06-27.
//

#include "materials/matte.h"
#include "interaction.h"
#include "reflection.h"
#include "texture.h"

namespace pbrt{

    void MatteMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena, TransportMode mode,
                                                   bool allowMultipleLobes) const {
        si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
        Spectrum r = Kd->Evaluate(*si).Clamp();
        float sig = Clamp(sigma->Evaluate(*si), 0, 90);
        if (!r.IsBlack()) {
            if (sig == 0)
                si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(r));
            else
                si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(r));
        }
    }
}