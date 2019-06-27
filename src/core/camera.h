//
// Created by Thierry Backes on 2019-06-18.
//

#ifndef PBRT_WHITTED_CAMERA_H
#define PBRT_WHITTED_CAMERA_H

#include "main.h"
#include "geometry.h"
#include "transform.h"
#include "film.h"

namespace pbrt {
class Camera {
    public:
    Camera(const Transform &CameraToWorld, Film *film): CameraToWorld(CameraToWorld),film(film){};
    virtual ~Camera();
    // return value says how much radiance along the ray will contribute to the final image
    virtual float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
    // computes ray + shifted rays (i.e. differentials)
    virtual float GenerateRayDifferential(const CameraSample &sample,
                                          RayDifferential *rd) const;
    Transform CameraToWorld;
    Film *film;


};

    struct CameraSample {
        Point2f pFilm;
        Point2f pLens;
    };

class ProjectiveCamera : public Camera {
public:
    ProjectiveCamera(const Transform &CameraToWorld,
                     const Transform &CameraToScreen,
                     const Bounds2f &screenWindow, Film *film)
            : Camera(CameraToWorld, film),
              CameraToScreen(CameraToScreen) {
        ScreenToRaster =
                Scale(film->fullResolution.x, film->fullResolution.y, 1) *
                Scale(1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                      1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
                Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));
        RasterToScreen = Inverse(ScreenToRaster);
        RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
                }
protected:
    Transform CameraToScreen, RasterToCamera;
    Transform ScreenToRaster, RasterToScreen;

};


}
#endif //PBRT_WHITTED_CAMERA_H
