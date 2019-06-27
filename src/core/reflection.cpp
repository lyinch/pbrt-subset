//
// Created by Thierry Backes on 2019-06-27.
//

#include "reflection.h"
namespace pbrt{

    Spectrum LambertianReflection::f(const Vector3f &wo, const Vector3f &wi) const {
        return R * InvPi;
    }

    Spectrum BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const {
        Vector3f wi = WorldToLocal(wiW), wo = WorldToLocal(woW);
        if (wo.z == 0) return 0.;
        bool reflect = Dot(wiW, ng) * Dot(woW, ng) > 0;
        Spectrum f(0.f);
        for (int i = 0; i < nBxDFs; ++i)
            if (bxdfs[i]->MatchesFlags(flags) &&
                ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                 (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
                f += bxdfs[i]->f(wo, wi);
        return f;
    }
}