//
// Created by Thierry Backes on 2019-06-27.
//

#ifndef PBRT_WHITTED_REFLECTION_H
#define PBRT_WHITTED_REFLECTION_H

#include "main.h"
#include "geometry.h"
#include "shape.h"
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

    inline float AbsCosTheta(const Vector3f &w) { return std::abs(w.z); }
    inline bool SameHemisphere(const Vector3f &w, const Vector3f &wp) {
        return w.z * wp.z > 0;
    }

    class BSDF {
    public:
        BSDF(const SurfaceInteraction &si, float eta = 1)
                : eta(eta),
                  ns(si.shading.n),
                  ng(si.n),
                  ss(Normalize(si.shading.dpdu)),
                  ts(Cross(ns, ss)) {}

        void Add(BxDF *b) {
            bxdfs[nBxDFs++] = b;
        }

        Vector3f WorldToLocal(const Vector3f &v) const {
            return Vector3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
        }
        Vector3f LocalToWorld(const Vector3f &v) const {
            return Vector3f(ss.x * v.x + ts.x * v.y + ns.x * v.z,
                            ss.y * v.x + ts.y * v.y + ns.y * v.z,
                            ss.z * v.x + ts.z * v.y + ns.z * v.z);
        }

        Spectrum f(const Vector3f &woW, const Vector3f &wiW,
                   BxDFType flags = BSDF_ALL) const;

        Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                          float *pdf, BxDFType type = BSDF_ALL,
                          BxDFType *sampledType = nullptr) const;

        const float eta;
    private:
        // BSDF Private Methods
        ~BSDF() {}

        // BSDF Private Data
        const Normal3f ns, ng;
        const Vector3f ss, ts;
        int nBxDFs = 0;
        static constexpr int MaxBxDFs = 8;
        BxDF *bxdfs[MaxBxDFs];
        friend class MixMaterial;
    };

    class BxDF {
    public:
        virtual ~BxDF(){}
        BxDF(BxDFType type) : type(type) {}
        bool MatchesFlags(BxDFType t) const { return (type & t) == type; }
        virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const = 0;
        virtual Spectrum rho(const Vector3f &wo, int nSamples,
                             const Point2f *samples) const;
        virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi,
                                  const Point2f &sample, float *pdf,
                                  BxDFType *sampledType = nullptr) const;
        virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

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
