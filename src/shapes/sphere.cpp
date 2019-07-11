//
// Created by Thierry Backes on 2019-06-26.
//

#include "sphere.h"
namespace pbrt{

    Bounds3f Sphere::ObjectBound() const {
        return Bounds3f(Point3f(-radius, -radius, zMin),
                        Point3f(radius, radius, zMax));
    }

    bool Sphere::Intersect(const Ray &r, float *tHit, SurfaceInteraction *isect, bool testAlphaTexture) const {
        float phi;
        Point3f pHit;
        Vector3f oErr, dErr;
        Ray ray = (*WorldToObject)(r);

        float ox(ray.o.x), oy(ray.o.y), oz(ray.o.z);
        float dx(ray.d.x), dy(ray.d.y), dz(ray.d.z);
        float a = dx * dx + dy * dy + dz * dz;
        float b = 2 * (dx * ox + dy * oy + dz * oz);
        float c = ox * ox + oy * oy + oz * oz - float(radius) * float(radius);

        float t0, t1;
        if (!Quadratic(a, b, c, &t0, &t1)) return false;

        if(t0  > ray.tMax || t1 <= 0) return false;
        float thit = t0;
        if(t0 <= 0){
            thit = t1;
            if(thit > ray.tMax) return false;
        }

        pHit = ray(thit);
        pHit *= radius / Distance(pHit, Point3f(0, 0, 0));
        if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5f * radius;

        phi = std::atan2(pHit.y, pHit.x);
        if (phi < 0) phi += 2 * Pi;


        // Find parametric representation of sphere hit
        float u = phi / phiMax;
        float theta = std::acos(Clamp(pHit.z / radius, -1, 1));
        float v = (theta - thetaMin) / (thetaMax - thetaMin);

        // Compute sphere $\dpdu$ and $\dpdv$
        float zRadius = std::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
        float invZRadius = 1 / zRadius;
        float cosPhi = pHit.x * invZRadius;
        float sinPhi = pHit.y * invZRadius;
        Vector3f dpdu(-phiMax * pHit.y, phiMax * pHit.x, 0);
        Vector3f dpdv =
                (thetaMax - thetaMin) *
                Vector3f(pHit.z * cosPhi, pHit.z * sinPhi, -radius * std::sin(theta));

        // Compute sphere $\dndu$ and $\dndv$
        Vector3f d2Pduu = -phiMax * phiMax * Vector3f(pHit.x, pHit.y, 0);
        Vector3f d2Pduv =
                (thetaMax - thetaMin) * pHit.z * phiMax * Vector3f(-sinPhi, cosPhi, 0.);
        Vector3f d2Pdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) *
                          Vector3f(pHit.x, pHit.y, pHit.z);

        // Compute coefficients for fundamental forms
        float E = Dot(dpdu, dpdu);
        float F = Dot(dpdu, dpdv);
        float G = Dot(dpdv, dpdv);
        Vector3f N = Normalize(Cross(dpdu, dpdv));
        float e = Dot(N, d2Pduu);
        float f = Dot(N, d2Pduv);
        float g = Dot(N, d2Pdvv);

        // Compute $\dndu$ and $\dndv$ from fundamental form coefficients
        float invEGF2 = 1 / (E * G - F * F);
        Normal3f dndu = Normal3f((f * F - e * G) * invEGF2 * dpdu +
                                 (e * F - f * E) * invEGF2 * dpdv);
        Normal3f dndv = Normal3f((g * F - f * G) * invEGF2 * dpdu +
                                 (f * F - g * E) * invEGF2 * dpdv);


        *isect = (*ObjectToWorld)(SurfaceInteraction(pHit, Point2f(u, v), -ray.d, dpdu, dpdv, dndu, dndv, this));

        // Update _tHit_ for quadric intersection
        *tHit = thit;
        return true;
    }

    std::shared_ptr<Shape> CreateSphereShape(const Transform *o2w, const Transform *w2o) {
        return std::make_shared<Sphere>(o2w, w2o, 0.3f, -0.3f,
                                        0.3f, 360.f);
    }
}