//
// Created by Thierry Backes on 2019-06-27.
//

#include "reflection.h"
#include "spectrum.h"
#include "sampler.h"
#include "sampling.h"
#include "scene.h"
#include "interaction.h"

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


    float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const {
        return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
    }

    Spectrum
    BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, float *pdf, BxDFType *sampledType) const {
        // Cosine-sample the hemisphere, flipping the direction if necessary
        *wi = CosineSampleHemisphere(sample);
        if (wo.z < 0) wi->z *= -1;
        *pdf = Pdf(wo, *wi);
        return f(wo, *wi);
    }

    Spectrum BxDF::rho(const Vector3f &w, int nSamples, const Point2f *u) const {
        Spectrum r(0.);
        for (int i = 0; i < nSamples; ++i) {
            // Estimate one term of $\rho_\roman{hd}$
            Vector3f wi;
            float pdf = 0;
            Spectrum f = Sample_f(w, &wi, u[i], &pdf);
            if (pdf > 0) r += f * AbsCosTheta(wi) / pdf;
        }
        return r / nSamples;
    }


}