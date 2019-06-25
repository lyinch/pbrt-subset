//
// Created by Thierry Backes on 2019-06-18.
//

#ifndef PBRT_WHITTED_CAMERA_H
#define PBRT_WHITTED_CAMERA_H

#include "geometry.h"
#include "main.h"

namespace pbrt {
class Camera {
    public:
    Camera(const AnimatedTransform &CameraToWorld, Film *film): CameraToWorld(CameraToWorld),film(film);
    virtual ~Camera();
    // return value says how much radiance along the ray will contribute to the final image
    virtual float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
    // computes ray + shifted rays (i.e. differentials)
    virtual float GenerateRayDifferential(const CameraSample &sample,
                                          RayDifferential *rd) const;
    AnimatedTransform CameraToWorld;
    Film *film;


};

    struct CameraSample {
        Point2f pFilm;
        Point2f pLens;
    };

class ProjectiveCamera : public Camera {
public:

};


}
#endif //PBRT_WHITTED_CAMERA_H
