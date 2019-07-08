//
// Created by Thierry Backes on 2019-06-18.
//

#include "camera.h"
#include "sampler.h"
#include "sampling.h"

namespace pbrt {
    Camera::~Camera() {
        delete film;
    }

}