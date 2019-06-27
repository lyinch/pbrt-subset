//
// Created by Thierry Backes on 2019-06-19.
//

#include "transform.h"
#include <memory>

namespace pbrt{

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

    Transform Orthographic(float zNear, float zFar) {
        return Scale(1, 1, 1 / (zFar - zNear)) * Translate(Vector3f(0, 0, -zNear));
    }
}