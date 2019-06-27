//
// Created by Thierry Backes on 2019-06-19.
//

#include "orthographic.h"
#include "sampler.h"

namespace pbrt {
    float OrthographicCamera::GenerateRay(const pbrt::CameraSample &sample, pbrt::Ray * ray) const {
        Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
        Point3f pCamera = RasterToCamera(pFilm);
        *ray = Ray(pCamera, Vector3f(0, 0, 1));
        *ray = CameraToWorld(*ray);
        return 1;
    }

}