//
// Created by Thierry Backes on 2019-06-23.
//

#ifndef PBRT_WHITTED_MEMORY_H
#define PBRT_WHITTED_MEMORY_H

#include "main.h"

namespace pbrt{
    class MemoryArena{
    public:
        MemoryArena(size_t blockSize = 262144) : blockSize(blockSize) {}

    private:
        const size_t blockSize;
    };
}
#endif //PBRT_WHITTED_MEMORY_H
