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

        float LengthSquared() const { return x * x + y * y; }
        float Length() const { return std::sqrt(LengthSquared()); }

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

        Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }

        template <typename U>
        Vector3<T> operator/(U f) const {
            float inv = (float)1 / f;
            return Vector3<T>(x * inv, y * inv, z * inv);
        }

        T operator[](int i) const {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        explicit Vector3(const Normal3<T> &n);

        float LengthSquared() const { return x * x + y * y+z*z; }
        float Length() const { return std::sqrt(LengthSquared()); }

        T x, y, z;
    };

    typedef Vector2<float> Vector2f;
    typedef Vector2<int> Vector2i;
    typedef Vector3<float> Vector3f;
    typedef Vector3<int> Vector3i;

    template <typename T>
    inline Vector3<T>::Vector3(const Normal3<T> &n)
            : x(n.x), y(n.y), z(n.z) {
    }

    template <typename T, typename U>
    inline Vector3<T> operator*(U s, const Vector3<T> &v) {
        return v * s;
    }
    template <typename T>
    Vector3<T> Abs(const Vector3<T> &v) {
        return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
    }

    template <typename T>
    inline T Dot(const Vector3<T> &v1, const Vector3<T> &v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }


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

        template <typename U>
        Point2<T> operator*(U f) const {
            return Point2<T>(f * x, f * y);
        }

        Vector2<T> operator-(const Point2<T> &p) const {
            return Vector2<T>(x - p.x, y - p.y);
        }

        bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
        bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }

        T x, y;

    };

    template<typename T>
    class Point3 {
    public:
        Point3() { x = y = z = 0; }
        Point3(T x, T y, T z) : x(x), y(y), z(z) {}
        template <typename U>
        explicit operator Vector3<U>() const {
            return Vector3<U>(x, y, z);
        }
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

        template <typename U>
        Point3<T> operator/(U f) const {
            float inv = (float)1 / f;
            return Point3<T>(inv * x, inv * y, inv * z);
        }

        T operator[](int i) const {
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        T &operator[](int i) {
            assert(i >= 0 && i <= 2);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
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

    template <typename T, typename U>
    inline Point2<T> operator*(U f, const Point2<T> &p) {
        return p * f;
    }

    template <typename T>
    class Normal3 {
    public:
        Normal3() { x = y = z = 0; }
        Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { }

        explicit Normal3<T>(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {
        }

        Normal3<T> operator+(const Normal3<T> &n) const {
            return Normal3<T>(x + n.x, y + n.y, z + n.z);
        }

        template <typename U>
        Normal3<T> operator*(U f) const {
            return Normal3<T>(f * x, f * y, f * z);
        }
        T x, y, z;

    };

    typedef Normal3<float> Normal3f;

    template <typename T, typename U>
    inline Normal3<T> operator*(U f, const Normal3<T> &n) {
        return Normal3<T>(f * n.x, f * n.y, f * n.z);
    }

    template <typename T>
    inline T Dot(const Vector3<T> &v1, const Normal3<T> &n2) {
        return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
    }

    template <typename T>
    inline T Dot(const Normal3<T> &n1, const Vector3<T> &v2) {
        return n1.x * v2.x + n1.y * v2.y + n1.z * v2.z;
    }

    template <typename T>
    Normal3<T> Abs(const Normal3<T> &v) {
        return Normal3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
    }

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
    public:
        RayDifferential() { hasDifferentials = false; }
        RayDifferential(const Point3f &o, const Vector3f &d, float tMax = Infinity)
                : Ray(o, d, tMax) {
            hasDifferentials = false;
        }
        void ScaleDifferentials(float s) {
            rxOrigin = o + (rxOrigin - o) * s;
            ryOrigin = o + (ryOrigin - o) * s;
            rxDirection = d + (rxDirection - d) * s;
            ryDirection = d + (ryDirection - d) * s;
        }
        RayDifferential(const Ray &ray) : Ray(ray) { hasDifferentials = false; }
        bool hasDifferentials;
        Point3f rxOrigin, ryOrigin;
        Vector3f rxDirection, ryDirection;
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


        template <typename U>
        explicit operator Bounds2<U>() const {
            return Bounds2<U>((Point2<U>)pMin, (Point2<U>)pMax);
        }

        bool operator==(const Bounds2<T> &b) const {
            return b.pMin == pMin && b.pMax == pMax;
        }
        bool operator!=(const Bounds2<T> &b) const {
            return b.pMin != pMin || b.pMax != pMax;
        }

        Vector2<T> Diagonal() const { return pMax - pMin; }

        T Area() const {
            Vector2<T> d = pMax - pMin;
            return (d.x * d.y);
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


    class Bounds2iIterator : public std::forward_iterator_tag {
    public:
        Bounds2iIterator(const Bounds2i &b, const Point2i &pt)
                : p(pt), bounds(&b) {}
        Bounds2iIterator operator++() {
            advance();
            return *this;
        }
        Bounds2iIterator operator++(int) {
            Bounds2iIterator old = *this;
            advance();
            return old;
        }
        bool operator==(const Bounds2iIterator &bi) const {
            return p == bi.p && bounds == bi.bounds;
        }
        bool operator!=(const Bounds2iIterator &bi) const {
            return p != bi.p || bounds != bi.bounds;
        }

        Point2i operator*() const { return p; }

    private:
        void advance() {
            ++p.x;
            if (p.x == bounds->pMax.x) {
                p.x = bounds->pMin.x;
                ++p.y;
            }
        }
        Point2i p;
        const Bounds2i *bounds;
    };

    inline Bounds2iIterator begin(const Bounds2i &b) {
        return Bounds2iIterator(b, b.pMin);
    }

    inline Bounds2iIterator end(const Bounds2i &b) {
        // Normally, the ending point is at the minimum x value and one past
        // the last valid y value.
        Point2i pEnd(b.pMin.x, b.pMax.y);
        // However, if the bounds are degenerate, override the end point to
        // equal the start point so that any attempt to iterate over the bounds
        // exits out immediately.
        if (b.pMin.x >= b.pMax.x || b.pMin.y >= b.pMax.y)
            pEnd = b.pMin;
        return Bounds2iIterator(b, pEnd);
    }

    template <typename T>
    Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2) {
        // Important: assign to pMin/pMax directly and don't run the Bounds2()
        // constructor, since it takes min/max of the points passed to it.  In
        // turn, that breaks returning an invalid bound for the case where we
        // intersect non-overlapping bounds (as we'd like to happen).
        Bounds2<T> ret;
        ret.pMin = Max(b1.pMin, b2.pMin);
        ret.pMax = Min(b1.pMax, b2.pMax);
        return ret;
    }

    inline Point3f OffsetRayOrigin(const Point3f &p, const Normal3f &n, const Vector3f &w) {
        Vector3f pError = Vector3f(1,1,1);
        float d = Dot(Abs(n),pError);

        Vector3f offset = d * Vector3f(n);
        if (Dot(w, n) < 0) offset = -offset;
        Point3f po = p + offset;
        // Round offset point _po_ away from _p_
        for (int i = 0; i < 3; ++i) {
            if (offset[i] > 0)
                po[i] = NextFloatUp(po[i]);
            else if (offset[i] < 0)
                po[i] = NextFloatDown(po[i]);
        }
        return po;
    }

    template <typename T>
    bool InsideExclusive(const Point2<T> &pt, const Bounds2<T> &b) {
        return (pt.x >= b.pMin.x && pt.x < b.pMax.x && pt.y >= b.pMin.y &&
                pt.y < b.pMax.y);
    }

    template <typename T>
    inline float DistanceSquared(const Point3<T> &p1, const Point3<T> &p2) {
        return (p1 - p2).LengthSquared();
    }

}
#endif //PBRT_WHITTED_GEOMETRY_H
