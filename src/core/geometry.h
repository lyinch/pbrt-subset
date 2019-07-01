//
// Created by Thierry Backes on 2019-06-17.
//

#ifndef PBRT_WHITTED_GEOMETRY_H
#define PBRT_WHITTED_GEOMETRY_H

#include <cassert>
#include "main.h"

namespace pbrt {
    template<typename T>
    class Vector2 {
    public:
        Vector2() { x = y = 0; }

        Vector2(T xx, T yy) : x(xx), y(yy) {}

        Vector2<T> operator+(const Vector2<T> &v) const {
            return Vector2(x + v.x, y + v.y);
        }

        Vector2<T> operator-(const Vector2<T> &v) const {
            return Vector2(x - v.x, y - v.y);
        }

        bool operator==(const Vector2<T> &v) const { return x == v.x && y == v.y; }

        bool operator!=(const Vector2<T> &v) const { return x != v.x || y != v.y; }

        template<typename U>
        Vector2<T> operator*(U f) const {
            return Vector2<T>(f * x, f * y);
        }

        T operator[](int i) const {
            assert(i >= 0 && i <= 1);
            if (i == 0) return x;
            return y;
        }


        T x, y;
    };

    template<typename T>
    class Vector3 {
    public:
        Vector3() { x = y = 0; }

        Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

        Vector3<T> operator+(const Vector3<T> &v) const {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        Vector3<T> operator-(const Vector3<T> &v) const {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        bool operator==(const Vector3<T> &v) const { return x == v.x && y == v.y && z == v.z; }

        bool operator!=(const Vector3<T> &v) const { return x != v.x || y != v.y || z != v.z; }

        template<typename U>
        Vector3<T> operator*(U f) const {
            return Vector3<T>(f * x, f * y, f * z);
        }

        T operator[](int i) const {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T x, y, z;
    };

    typedef Vector2<float> Vector2f;
    typedef Vector2<int> Vector2i;
    typedef Vector3<float> Vector3f;
    typedef Vector3<int> Vector3i;

    template<typename T>
    class Point2 {
    public:
        Point2() { x = y = 0; }
        Point2(T xx, T yy) : x(xx), y(yy) {}

        explicit Point2(const Point3<T> &p) : x(p.x), y(p.y) {}

        template<typename U>
        explicit Point2(const Point2<U> &p) {
            x = (T) p.x;
            y = (T) p.y;
        }

        template<typename U>
        explicit Point2(const Vector2<U> &v) {
            x = (T) v.x;
            y = (T) v.y;
        }


        Point2<T> operator-(const Vector2<T> &v) const {
            return Point2<T>(x - v.x, y - v.y);
        }

        Point2<T> operator+(const Vector2<T> &v) const {
            return Point2<T>(x + v.x, y + v.y);
        }

        Point2<T> operator+(const Point2<T> &p) const {
            return Point2<T>(x + p.x, y + p.y);
        }

        T x, y;

    };

    template<typename T>
    class Point3 {
    public:
        Point3() { x = y = z = 0; }
        Point3(T x, T y, T z) : x(x), y(y), z(z) {}

        T x, y, z;

        Point3<T> &operator+=(const Vector3<T> &v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        Point3<T> operator+(const Vector3<T> &v) const {
            return Point3<T>(x + v.x, y + v.y, z + v.z);
        }
        Vector3<T> operator-(const Point3<T> &p) const {
            return Vector3<T>(x - p.x, y - p.y, z - p.z);
        }
    };

    typedef Point2<float> Point2f;
    typedef Point2<int> Point2i;
    typedef Point3<float> Point3f;
    typedef Point3<int> Point3i;

    template <typename T>
    Point2<T> Min(const Point2<T> &pa, const Point2<T> &pb) {
        return Point2<T>(std::min(pa.x, pb.x), std::min(pa.y, pb.y));
    }

    template <typename T>
    Point2<T> Max(const Point2<T> &pa, const Point2<T> &pb) {
        return Point2<T>(std::max(pa.x, pb.x), std::max(pa.y, pb.y));
    }

    template <typename T>
    Point2<T> Floor(const Point2<T> &p) {
        return Point2<T>(std::floor(p.x), std::floor(p.y));
    }

    template <typename T>
    Point2<T> Ceil(const Point2<T> &p) {
        return Point2<T>(std::ceil(p.x), std::ceil(p.y));
    }

    template <typename T>
    class Normal3 {
    public:
        Normal3() { x = y = z = 0; }
        Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { }

        T x, y, z;

    };

    typedef Normal3<float> Normal3f;

    template <typename T>
    inline Vector3<T> Cross(const Vector3<T> &v1, const Vector3<T> &v2) {
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;
        return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
                          (v1x * v2y) - (v1y * v2x));
    }

    template <typename T>
    inline Vector3<T> Cross(const Vector3<T> &v1, const Normal3<T> &v2) {
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;
        return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
                          (v1x * v2y) - (v1y * v2x));
    }

    template <typename T>
    inline Vector3<T> Cross(const Normal3<T> &v1, const Vector3<T> &v2) {
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;
        return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
                          (v1x * v2y) - (v1y * v2x));
    }

    template <typename T>
    inline Vector3<T> Normalize(const Vector3<T> &v) {
        return v / v.Length();
    }


    class Ray {
    public:
        Ray() : tMax(Infinity){}
        Ray(const Point3f &o, const Vector3f &d, float tMax = Infinity)
                : o(o), d(d), tMax(tMax){}
        Point3f operator()(float t) const { return o + d * t; }
        Point3f o;
        Vector3f d;
        mutable float tMax;
    };

    class RayDifferential : public Ray {

    };

    template <typename T>
    inline T AbsDot(const Vector3<T> &v1, const Vector3<T> &v2) {
        return std::abs(Dot(v1, v2));
    }

    template <typename T>
    inline T AbsDot(const Normal3<T> &n1, const Vector3<T> &v2) {
        return std::abs(n1.x * v2.x + n1.y * v2.y + n1.z * v2.z);
    }

    template <typename T>
    inline T AbsDot(const Vector3<T> &v1, const Normal3<T> &n2) {
        return std::abs(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
    }

    template <typename T>
    class Bounds2 {
        public:
        Bounds2() {
            T minNum = std::numeric_limits<T>::lowest();
            T maxNum = std::numeric_limits<T>::max();
            pMin = Point2<T>(maxNum, maxNum);
            pMax = Point2<T>(minNum, minNum);
        }
        explicit Bounds2(const Point2<T> &p) : pMin(p), pMax(p) {}
        Bounds2(const Point2<T> &p1, const Point2<T> &p2) {
            pMin = Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
            pMax = Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
        }

        T Area() const {
            Vector2<T> d = pMax - pMin;
            return (d.x * d.y);
        }

        template <typename U>
        explicit operator Bounds2<U>() const {
            return Bounds2<U>((Point2<U>)pMin, (Point2<U>)pMax);
        }


        Point2<T> pMin, pMax;


    };

    template <typename T>
    class Bounds3 {

    };

    typedef Bounds2<float> Bounds2f;
    typedef Bounds2<int> Bounds2i;
    typedef Bounds3<float> Bounds3f;
    typedef Bounds3<int> Bounds3i;


}
#endif //PBRT_WHITTED_GEOMETRY_H
