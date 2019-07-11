//
// Created by Thierry Backes on 2019-06-19.
//

#include "transform.h"
#include "interaction.h"

namespace pbrt{

    bool SolveLinearSystem2x2(const float A[2][2], const float B[2], float *x0,
                              float *x1) {
        float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
        if (std::abs(det) < 1e-10f) return false;
        *x0 = (A[1][1] * B[0] - A[0][1] * B[1]) / det;
        *x1 = (A[0][0] * B[1] - A[1][0] * B[0]) / det;
        if (std::isnan(*x0) || std::isnan(*x1)) return false;
        return true;
    }

    Matrix4x4::Matrix4x4(float mat[4][4]) { memcpy(m, mat, 16 * sizeof(float)); }
    Matrix4x4::Matrix4x4(float t00, float t01, float t02, float t03, float t10,
                         float t11, float t12, float t13, float t20, float t21,
                         float t22, float t23, float t30, float t31, float t32,
                         float t33) {
        m[0][0] = t00;
        m[0][1] = t01;
        m[0][2] = t02;
        m[0][3] = t03;
        m[1][0] = t10;
        m[1][1] = t11;
        m[1][2] = t12;
        m[1][3] = t13;
        m[2][0] = t20;
        m[2][1] = t21;
        m[2][2] = t22;
        m[2][3] = t23;
        m[3][0] = t30;
        m[3][1] = t31;
        m[3][2] = t32;
        m[3][3] = t33;
    }

    Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up) {
        Matrix4x4 cameraToWorld;
        cameraToWorld.m[0][3] = pos.x;
        cameraToWorld.m[1][3] = pos.y;
        cameraToWorld.m[2][3] = pos.z;
        cameraToWorld.m[3][3] = 1;

        Vector3f dir = Normalize(look - pos);
        Vector3f right = Normalize(Cross(Normalize(up), dir));
        Vector3f newUp = Cross(dir, right);
        cameraToWorld.m[0][0] = right.x;
        cameraToWorld.m[1][0] = right.y;
        cameraToWorld.m[2][0] = right.z;
        cameraToWorld.m[3][0] = 0.;
        cameraToWorld.m[0][1] = newUp.x;
        cameraToWorld.m[1][1] = newUp.y;
        cameraToWorld.m[2][1] = newUp.z;
        cameraToWorld.m[3][1] = 0.;
        cameraToWorld.m[0][2] = dir.x;
        cameraToWorld.m[1][2] = dir.y;
        cameraToWorld.m[2][2] = dir.z;
        cameraToWorld.m[3][2] = 0.;
        return Transform(Inverse(cameraToWorld), cameraToWorld);
    }

    Transform Translate(const Vector3f &delta) {
        Matrix4x4 m(1, 0, 0, delta.x, 0, 1, 0, delta.y, 0, 0, 1, delta.z, 0, 0, 0,
                    1);
        Matrix4x4 minv(1, 0, 0, -delta.x, 0, 1, 0, -delta.y, 0, 0, 1, -delta.z, 0,
                       0, 0, 1);
        return Transform(m, minv);
    }

    Transform Scale(float x, float y, float z) {
        Matrix4x4 m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
        Matrix4x4 minv(1 / x, 0, 0, 0, 0, 1 / y, 0, 0, 0, 0, 1 / z, 0, 0, 0, 0, 1);
        return Transform(m, minv);
    }

    Transform Transform::operator*(const Transform &t2) const {
        return Transform(Matrix4x4::Mul(m, t2.m), Matrix4x4::Mul(t2.mInv, mInv));
    }

    SurfaceInteraction Transform::operator()(const SurfaceInteraction &si) const {
        SurfaceInteraction ret;
        // Transform _p_ and _pError_ in _SurfaceInteraction_
        ret.p = (*this)(si.p);

        // Transform remaining members of _SurfaceInteraction_
        const Transform &t = *this;
        ret.n = Normalize(t(si.n));
        ret.wo = Normalize(t(si.wo));
        ret.uv = si.uv;
        ret.shape = si.shape;
        ret.dpdu = t(si.dpdu);
        ret.dpdv = t(si.dpdv);
        ret.dndu = t(si.dndu);
        ret.dndv = t(si.dndv);
        ret.shading.n = Normalize(t(si.shading.n));
        ret.shading.dpdu = t(si.shading.dpdu);
        ret.shading.dpdv = t(si.shading.dpdv);
        ret.shading.dndu = t(si.shading.dndu);
        ret.shading.dndv = t(si.shading.dndv);
        ret.dudx = si.dudx;
        ret.dvdx = si.dvdx;
        ret.dudy = si.dudy;
        ret.dvdy = si.dvdy;
        ret.dpdx = t(si.dpdx);
        ret.dpdy = t(si.dpdy);
        ret.bsdf = si.bsdf;
        ret.primitive = si.primitive;
        //    ret.n = Faceforward(ret.n, ret.shading.n);
        //ret.shading.n = Faceforward(ret.shading.n, ret.n);
        return ret;
    }

    template<typename T>
    Normal3<T> Transform::operator()(const Normal3<T> &n) const {
        T x = n.x, y = n.y, z = n.z;
        return Normal3<T>(mInv.m[0][0] * x + mInv.m[1][0] * y + mInv.m[2][0] * z,
                          mInv.m[0][1] * x + mInv.m[1][1] * y + mInv.m[2][1] * z,
                          mInv.m[0][2] * x + mInv.m[1][2] * y + mInv.m[2][2] * z);    }

    Bounds3f Transform::operator()(const Bounds3f &b) const {
        const Transform &M = *this;
        Bounds3f ret(M(Point3f(b.pMin.x, b.pMin.y, b.pMin.z)));
        ret = Union(ret, M(Point3f(b.pMax.x, b.pMin.y, b.pMin.z)));
        ret = Union(ret, M(Point3f(b.pMin.x, b.pMax.y, b.pMin.z)));
        ret = Union(ret, M(Point3f(b.pMin.x, b.pMin.y, b.pMax.z)));
        ret = Union(ret, M(Point3f(b.pMin.x, b.pMax.y, b.pMax.z)));
        ret = Union(ret, M(Point3f(b.pMax.x, b.pMax.y, b.pMin.z)));
        ret = Union(ret, M(Point3f(b.pMax.x, b.pMin.y, b.pMax.z)));
        ret = Union(ret, M(Point3f(b.pMax.x, b.pMax.y, b.pMax.z)));
        return ret;
    }

    Transform Orthographic(float zNear, float zFar) {
        return Scale(1, 1, 1 / (zFar - zNear)) * Translate(Vector3f(0, 0, -zNear));
    }
}