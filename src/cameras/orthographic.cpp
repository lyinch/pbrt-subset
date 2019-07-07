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

    OrthographicCamera *CreateOrthographicCamera(const Transform &cam2world, Film *film) {
        float frame = float(film->fullResolution.x)/float(film->fullResolution.y);

        Bounds2f screen;
        if (frame > 1.f) {
            screen.pMin.x = -frame;
            screen.pMax.x = frame;
            screen.pMin.y = -1.f;
            screen.pMax.y = 1.f;
        } else {
            screen.pMin.x = -1.f;
            screen.pMax.x = 1.f;
            screen.pMin.y = -1.f / frame;
            screen.pMax.y = 1.f / frame;
        }

        return new OrthographicCamera(cam2world,screen,film);
    }
}