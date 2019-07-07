//
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_TRANSFORM_H
#define PBRT_WHITTED_TRANSFORM_H

#include "geometry.h"

namespace pbrt{
    struct Matrix4x4 {
        Matrix4x4() {
            m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
            m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] =
            m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.f;
        }
        Matrix4x4(float mat[4][4]);
        Matrix4x4(float t00, float t01, float t02, float t03, float t10, float t11,
                  float t12, float t13, float t20, float t21, float t22, float t23,
                  float t30, float t31, float t32, float t33);
        friend Matrix4x4 Transpose(const Matrix4x4 &);
        friend Matrix4x4 Inverse(const Matrix4x4 &);

        static Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2) {
            Matrix4x4 r;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    r.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
                                m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
            return r;
        }
        bool operator==(const Matrix4x4 &m2) const {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    if (m[i][j] != m2.m[i][j]) return false;
            return true;
        }

        float m[4][4];
    };
        class Transform {
        public:
            Transform() {}
            Transform(const float mat[4][4]) {
                m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0],
                              mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1],
                              mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2],
                              mat[3][3]);
                mInv = Inverse(m);
            }
            Transform(const Matrix4x4 &m) : m(m), mInv(Inverse(m)) {}
            Transform(const Matrix4x4 &m, const Matrix4x4 &mInv) : m(m), mInv(mInv) {}
            friend Transform Inverse(const Transform &t) {
                return Transform(t.mInv, t.m);
            }

            template <typename T>
            inline Point3<T> operator()(const Point3<T> &p) const;
            template <typename T>
            inline Vector3<T> operator()(const Vector3<T> &v) const;
            inline Ray operator()(const Ray &r) const;

            Transform operator*(const Transform &t2) const;
            bool operator==(const Transform &t) const {
                return t.m == m && t.mInv == mInv;
            }
        private:
            // Transform Private Data
            Matrix4x4 m, mInv;
    };

    Transform Translate(const Vector3f &delta);
    Transform Scale(float x, float y, float z);
    Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up);
    Transform Orthographic(float znear, float zfar);

    template <typename T>
    inline Point3<T> Transform::operator()(const Point3<T> &p) const {
        T x = p.x, y = p.y, z = p.z;
        T xp = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
        T yp = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
        T zp = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
        T wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
        if (wp == 1)
            return Point3<T>(xp, yp, zp);
        else
            return Point3<T>(xp, yp, zp) / wp;
    }

    template <typename T>
    inline Vector3<T> Transform::operator()(const Vector3<T> &v) const {
        T x = v.x, y = v.y, z = v.z;
        return Vector3<T>(m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
                          m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
                          m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
    }

    inline Ray Transform::operator()(const Ray &r) const {
        Point3f o = (*this)(r.o);
        Vector3f d = (*this)(r.d);
        // Offset ray origin to edge of error bounds and compute _tMax_
        float lengthSquared = d.LengthSquared();
        float tMax = r.tMax;
        if (lengthSquared > 0) {
            Vector3f oError(0,0,0);
            float dt = Dot(Abs(d),oError) / lengthSquared;
            o += d * dt;
            tMax -= dt;
        }
        return Ray(o, d, tMax);
    }

}
#endif //PBRT_WHITTED_TRANSFORM_H
