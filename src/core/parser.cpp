//
// Created by Thierry Backes on 2019-06-16.
//

#include <string>
#include "parser.h"
#include "api.h"
namespace pbrt {

    void parse() {
        /*
           LookAt 0 0 0  # eye
           0 0 -1  # look at point
           0 1 0    # up vector
         */
        pbrtLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
        pbrtCamera("orthographic");
        pbrtIntegrator("whitted");
        pbrtFilm("image");
        pbrtWorldBegin();
        // scene
        pbrtLightSource("point");
        pbrtTranslate(-0.1f,0,-1);
        pbrtShape("Sphere");

        // scene end
        pbrtWorldEnd();
    }

}