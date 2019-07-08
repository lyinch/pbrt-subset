//
// Created by Thierry Backes on 2019-07-07.
//

#ifndef PBRT_WHITTED_PARALLEL_H
#define PBRT_WHITTED_PARALLEL_H
#include "main.h"
#include "geometry.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace pbrt{
    class AtomicFloat {
    public:
        // AtomicFloat Public Methods
        explicit AtomicFloat(float v = 0) { bits = FloatToBits(v); }
        operator float() const { return BitsToFloat(bits); }
        float operator=(float v) {
            bits = FloatToBits(v);
            return v;
        }
        void Add(float v) {

            uint32_t oldBits = bits, newBits;
            do {
                newBits = FloatToBits(BitsToFloat(oldBits) + v);
            } while (!bits.compare_exchange_weak(oldBits, newBits));
        }

    private:
// AtomicFloat Private Data
        std::atomic<uint32_t> bits;
    };

    int MaxThreadIndex();
    int NumSystemCores();
    void ParallelFor2D(std::function<void(Point2i)> func, const Point2i &count);
}


#endif //PBRT_WHITTED_PARALLEL_H
