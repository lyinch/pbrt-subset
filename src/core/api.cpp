//
// Created by Thierry Backes on 2019-06-19.
//
#include <samplers/random.h>
#include <filters/box.h>
#include "api.h"
#include "camera.h"
#include "scene.h"
#include "integrator.h"
#include "spectrum.h"
#include "filter.h"
#include "transform.h"
#include "film.h"
#include "light.h"

namespace pbrt {
    Options PbrtOptions;
    constexpr int MaxTransforms = 2;
    constexpr int AllTransformsBits = (1 << MaxTransforms) - 1;

    struct TransformSet{
        Transform &operator[](int i) {
            return t[i];
        }

        const Transform &operator[](int i) const {
            return t[i];
        }

        friend TransformSet Inverse(const TransformSet &ts) {
            TransformSet tInv;
            for (int i = 0; i < MaxTransforms; ++i) tInv.t[i] = Inverse(ts.t[i]);
            return tInv;
        }
    private:
        Transform t[MaxTransforms];
    };

    struct RenderOptions {
        Integrator *MakeIntegrator() const;

        Scene *MakeScene();

        Camera *MakeCamera() const;

        std::string FilterName = "box";
        std::string FilmName = "image";
        std::string SamplerName = "random";
        std::string IntegratorName = "whitted";
        std::string CameraName = "orthographic";
    };

    struct GraphicsState {

    };

    // API Static Data
    static GraphicsState graphicsState;
    static std::unique_ptr<RenderOptions> renderOptions;
    static TransformSet curTransform;
    static uint32_t activeTransformBits = AllTransformsBits;

#define FOR_ACTIVE_TRANSFORMS(expr)           \
    for (int i = 0; i < MaxTransforms; ++i)   \
        if (activeTransformBits & (1 << i)) { \
            expr                              \
        }

    void pbrtLookAt(float ex, float ey, float ez, float lx, float ly, float lz,
                    float ux, float uy, float uz) {
        Transform lookAt = LookAt(Point3f(ex,ey,ez),Point3f(lx,ly,lz),Vector3f(ux,uy,uz));
        FOR_ACTIVE_TRANSFORMS(curTransform[i] = curTransform[i] * lookAt;);
    }

    void pbrtInit(const Options &opt) {
        PbrtOptions = opt;
        renderOptions.reset(new RenderOptions);
        graphicsState = GraphicsState();
    }

    void pbrtCamera(const std::string &name) {
        renderOptions->CameraName = name;
    }

    void pbrtIntegrator(const std::string &name) {
        renderOptions->IntegratorName = name;

    }

    void pbrtFilm(const std::string &type) {
        renderOptions->FilmName = type;
    }

    void pbrtWorldBegin() {
        for (int i = 0; i < MaxTransforms; ++i) curTransform[i] = Transform();
        activeTransformBits = AllTransformsBits;
        namedCoordinateSystems["world"] = curTransform;
    }

    void pbrtWorldEnd() {
        std::unique_ptr<Integrator> integrator(renderOptions->MakeIntegrator());
        std::unique_ptr<Scene> scene(renderOptions->MakeScene());
        if (scene && integrator) integrator->Render(*scene);

    }

    void pbrtLightSource(const std::string &name) {
        std::shared_ptr<Light> lt = MakeLight(name, curTransform[0], mi);
        renderOptions->lights.push_back(lt);

    }

    void pbrtShape(const std::string &name) {


    }

    void pbrtTranslate(float dx, float dy, float dz) {
        FOR_ACTIVE_TRANSFORMS(curTransform[i] = curTransform[i] *
                                                Translate(Vector3f(dx, dy, dz));)
    }


    std::shared_ptr<Sampler> MakeSampler(const std::string &name,
                                         const Film *film) {
        Sampler *sampler = nullptr;
        if (name == "random")
            sampler = CreateRandomSampler();
        return std::shared_ptr<Sampler>(sampler);

    }

    std::unique_ptr<Filter> MakeFilter(const std::string &name) {
        Filter *filter = nullptr;
        if (name == "box")
            filter = CreateBoxFilter();
        return std::unique_ptr<Filter>(filter);
    }


    Film *MakeFilm(const std::string &name, std::unique_ptr<Filter> filter){
        Film *film = nullptr;
        if (name == "image")
            film = CreateFilm(std::move(filter));
        return film;
    }
    Integrator *RenderOptions::MakeIntegrator() const {
        std::shared_ptr<const Camera> camera(MakeCamera());
        std::shared_ptr<Sampler> sampler =
                MakeSampler(SamplerName, camera->film);
        Integrator *integrator = nullptr;
        if (IntegratorName == "whitted")
            integrator = CreateWhittedIntegrator(sampler, camera);
    }

    Camera *RenderOptions::MakeCamera() const {
        std::unique_ptr<Filter> filter = MakeFilter(FilterName);
        Film *film = MakeFilm(FilmName, std::move(filter));

    }

}