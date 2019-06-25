//
// Created by Thierry Backes on 2019-06-16.
//

#ifndef PBRT_WHITTED_API_H
#define PBRT_WHITTED_API_H

#include "main.h"

namespace pbrt{

    void pbrtInit(const Options &opt);
    void pbrtCleanup();
    void pbrtTranslate(float dx, float dy, float dz);
    void pbrtFilm(const std::string &type);
    void pbrtLookAt(float ex, float ey, float ez, float lx, float ly, float lz,
                    float ux, float uy, float uz);
    void pbrtIntegrator(const std::string &name);
    void pbrtCamera(const std::string &name);

    void pbrtWorldBegin();

    void pbrtLightSource(const std::string &name);
    void pbrtShape(const std::string &name);
    void pbrtWorldEnd();


}
#endif //PBRT_WHITTED_API_H
