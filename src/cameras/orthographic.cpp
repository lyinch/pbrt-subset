//
// Created by Thierry Backes on 2019-06-19.
//

#include "orthographic.h"
#include "sampler.h"

namespace pbrt {
    float OrthographicCamera::GenerateRay(const pbrt::CameraSample &sample, pbrt::Ray *) const {
        return 0;
    }

}