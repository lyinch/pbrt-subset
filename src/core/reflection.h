//
// Created by Thierry Backes on 2019-06-27.
//

#ifndef PBRT_WHITTED_REFLECTION_H
#define PBRT_WHITTED_REFLECTION_H

#include "main.h"
#include "geometry.h"
#include "spectrum.h"

namespace pbrt{

    enum BxDFType {
        BSDF_REFLECTION = 1 << 0,
        BSDF_TRANSMISSION = 1 << 1,
        BSDF_DIFFUSE = 1 << 2,
        BSDF_GLOSSY = 1 << 3,
        BSDF_SPECULAR = 1 << 4,
        BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION |
                   BSDF_TRANSMISSION,
    };

    class BxDF {
    public:
        virtual ~BxDF(){}
        BxDF(BxDFType type) : type(type) {}
        bool MatchesFlags(BxDFType t) const { return (type & t) == type; }
        virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const = 0;
        virtual Spectrum rho(const Vector3f &wo, int nSamples,
                             const Point2f *samples) const;

        const BxDFType type;
    };

    class LambertianReflection : public BxDF{
    public:
        LambertianReflection(const Spectrum &R)
                : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}
        Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
        Spectrum rho(const Vector3f &, int, const Point2f *) const { return R; }


    private:
        const Spectrum R;
    };

}
#endif //PBRT_WHITTED_REFLECTION_H
