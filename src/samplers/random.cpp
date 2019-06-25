//
// Created by Thierry Backes on 2019-06-18.
//

#include "random.h"
namespace pbrt {
    RandomSampler::RandomSampler(int ns, int seed) : Sampler(ns), rng(seed) {}

    std::unique_ptr<Sampler> RandomSampler::Clone(int seed) {
        auto *rs = new RandomSampler(*this);
        rs->rng.SetSequence(seed);
        return std::unique_ptr<Sampler>(rs);
    }

    Sampler *CreateRandomSampler() {
        return new RandomSampler(4);
    }
}
