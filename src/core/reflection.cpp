//
// Created by Thierry Backes on 2019-06-27.
//

#include "reflection.h"
namespace pbrt{

    Spectrum LambertianReflection::f(const Vector3f &wo, const Vector3f &wi) const {
        return R * InvPi;
    }
}