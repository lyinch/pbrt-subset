//
// Created by Thierry Backes on 2019-06-16.
//

#ifndef PBRT_WHITTED_SPECTRUM_H
#define PBRT_WHITTED_SPECTRUM_H

#include "main.h"

namespace pbrt {

    inline void XYZToRGB(const float xyz[3], float rgb[3]) {
        rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
        rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
        rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
    }

    inline void RGBToXYZ(const float rgb[3], float xyz[3]) {
        xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
        xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
        xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
    }

    enum class SpectrumType { Reflectance, Illuminant };


    static const int nCIESamples = 471;
    extern const float CIE_X[nCIESamples];
    extern const float CIE_Y[nCIESamples];
    extern const float CIE_Z[nCIESamples];
    extern const float CIE_lambda[nCIESamples];
    static const float CIE_Y_integral = 106.856895;

    template<int nSpectrumSamples>
    class CoefficientSpectrum {
    public:
        CoefficientSpectrum(float v = 0.f) {
            for (int i = 0; i < nSpectrumSamples; ++i) {
                c[i] = v;
            }
        }

        static const int nSamples = nSpectrumSamples;
        bool HasNaNs() const {
            for (int i = 0; i < nSpectrumSamples; ++i)
                if (std::isnan(c[i])) return true;
            return false;
        }
        CoefficientSpectrum &operator+=(const CoefficientSpectrum &s2) {
            for (int i = 0; i < nSpectrumSamples; ++i) c[i] += s2.c[i];
            return *this;
        }
        CoefficientSpectrum operator+(const CoefficientSpectrum &s2) const {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] += s2.c[i];
            return ret;
        }
        CoefficientSpectrum operator-(const CoefficientSpectrum &s2) const {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] -= s2.c[i];
            return ret;
        }
        CoefficientSpectrum operator/(const CoefficientSpectrum &s2) const {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i) {
                ret.c[i] /= s2.c[i];
            }
            return ret;
        }
        CoefficientSpectrum operator*(const CoefficientSpectrum &sp) const {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] *= sp.c[i];
            return ret;
        }
        CoefficientSpectrum &operator*=(const CoefficientSpectrum &sp) {
            for (int i = 0; i < nSpectrumSamples; ++i) c[i] *= sp.c[i];
            return *this;
        }
        CoefficientSpectrum operator*(float a) const {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] *= a;
            return ret;
        }
        CoefficientSpectrum &operator*=(float a) {
            for (int i = 0; i < nSpectrumSamples; ++i) c[i] *= a;
            return *this;
        }
        friend inline CoefficientSpectrum operator*(float a,
                                                    const CoefficientSpectrum &s) {
            return s * a;
        }
        CoefficientSpectrum operator/(float a) const {
            CoefficientSpectrum ret = *this;
            for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] /= a;
            return ret;
        }
        CoefficientSpectrum &operator/=(float a) {
            for (int i = 0; i < nSpectrumSamples; ++i) c[i] /= a;
            return *this;
        }
        bool operator==(const CoefficientSpectrum &sp) const {
            for (int i = 0; i < nSpectrumSamples; ++i)
                if (c[i] != sp.c[i]) return false;
            return true;
        }
        bool operator!=(const CoefficientSpectrum &sp) const {
            return !(*this == sp);
        }

        bool IsBlack() const {
            for (int i = 0; i < nSpectrumSamples; ++i)
                if (c[i] != 0.) return false;
            return true;
        }

        CoefficientSpectrum Clamp(float low = 0, float high = Infinity) const {
            CoefficientSpectrum ret;
            for (int i = 0; i < nSpectrumSamples; ++i)
                ret.c[i] = pbrt::Clamp(c[i], low, high);
            return ret;
        }

    protected:
        float c[nSpectrumSamples];
    };

    class RGBSpectrum : public CoefficientSpectrum<3> {
        using CoefficientSpectrum<3>::c;

    public:
        RGBSpectrum(float v = 0.f) : CoefficientSpectrum<3>(v) {}
        RGBSpectrum(const CoefficientSpectrum<3> &v) : CoefficientSpectrum<3>(v) {}
        RGBSpectrum(const RGBSpectrum &s,
                    SpectrumType type = SpectrumType::Reflectance) {
            *this = s;
        }

        static RGBSpectrum FromRGB(const float rgb[3],
                                   SpectrumType type = SpectrumType::Reflectance) {
            RGBSpectrum s;
            s.c[0] = rgb[0];
            s.c[1] = rgb[1];
            s.c[2] = rgb[2];
            return s;
        }

        void ToRGB(float *rgb) const {
            rgb[0] = c[0];
            rgb[1] = c[1];
            rgb[2] = c[2];
        }
        const RGBSpectrum &ToRGBSpectrum() const { return *this; }

        void ToXYZ(float xyz[3]) const { RGBToXYZ(c, xyz); }

        float y() const {
            const float YWeight[3] = {0.212671f, 0.715160f, 0.072169f};
            return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
        }

        static RGBSpectrum FromXYZ(const float xyz[3],
                                   SpectrumType type = SpectrumType::Reflectance) {
            RGBSpectrum r;
            XYZToRGB(xyz, r.c);
            return r;
        }
    };

}
#endif //PBRT_WHITTED_SPECTRUM_H
