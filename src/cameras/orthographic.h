//
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_ORTHOGRAPHIC_H
#define PBRT_WHITTED_ORTHOGRAPHIC_H

#include "camera.h"

namespace pbrt{
    class OrthographicCamera: public ProjectiveCamera{
    public:
        OrthographicCamera(const Transform &CameraToWorld,
                           const Bounds2f &screenWindow,
                           Film *film)
                : ProjectiveCamera(CameraToWorld, Orthographic(0, 1), screenWindow,
                                   film) {
            dxCamera = RasterToCamera(Vector3f(1, 0, 0));
            dyCamera = RasterToCamera(Vector3f(0, 1, 0));
        }

        float GenerateRay(const CameraSample &sample, Ray *) const;

    private:
        Vector3f dxCamera, dyCamera;
    };

    OrthographicCamera *CreateOrthographicCamera(const Transform &cam2world, Film *film);
}

#endif //PBRT_WHITTED_ORTHOGRAPHIC_H
