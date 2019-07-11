//
// Created by Thierry Backes on 2019-06-19.
//

#include "api.h"
#include "spectrum.h"
#include "scene.h"
#include "film.h"

#include "cameras/orthographic.h"
#include "filters/box.h"
#include "integrators/whitted.h"
#include "lights/point.h"
#include "materials/matte.h"
#include "samplers/random.h"
#include "shapes/sphere.h"
#include "textures/constant.h"

#include <map>


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
        TransformSet CameraToWorld;
        std::vector<std::shared_ptr<Primitive>> primitives;
        std::vector<std::shared_ptr<Light>> lights;

    };

    struct MaterialInstance {
        MaterialInstance() = default;
        MaterialInstance(const std::string &name, const std::shared_ptr<Material> &mtl)
                : name(name), material(mtl) {}

        std::string name;
        std::shared_ptr<Material> material;
    };

    struct GraphicsState {
        GraphicsState()
                : floatTextures(std::make_shared<FloatTextureMap>()),
                  spectrumTextures(std::make_shared<SpectrumTextureMap>()),
                  namedMaterials(std::make_shared<NamedMaterialMap>()) {
        }

        std::shared_ptr<Material> GetMaterialForShape();

        using FloatTextureMap = std::map<std::string, std::shared_ptr<Texture<float>>>;
        std::shared_ptr<FloatTextureMap> floatTextures;
        bool floatTexturesShared = false;

        using SpectrumTextureMap = std::map<std::string, std::shared_ptr<Texture<Spectrum>>>;
        std::shared_ptr<SpectrumTextureMap> spectrumTextures;
        bool spectrumTexturesShared = false;

        using NamedMaterialMap = std::map<std::string, std::shared_ptr<MaterialInstance>>;
        std::shared_ptr<NamedMaterialMap> namedMaterials;
        bool namedMaterialsShared = false;

        std::shared_ptr<MaterialInstance> currentMaterial;

    };

    std::shared_ptr<Material> GraphicsState::GetMaterialForShape(){
        return currentMaterial->material;
    }

            class TransformCache{
    public:
        TransformCache()
                : hashTable(512), hashTableOccupancy(0) {}

        Transform *Lookup(const Transform &t) {

            int offset = Hash(t) & (hashTable.size() - 1);
            int step = 1;
            while (true) {
                // Keep looking until we find the Transform or determine that
                // it's not present.
                if (!hashTable[offset] || *hashTable[offset] == t)
                    break;
                // Advance using quadratic probing.
                offset = (offset + step * step) & (hashTable.size() - 1);
                ++step;
            }
            Transform *tCached = hashTable[offset];
            if (tCached){}
            else {
                tCached = arena.Alloc<Transform>();
                *tCached = t;
                Insert(tCached);
            }
            return tCached;
        }

    private:
        void Insert(Transform *tNew);
        void Grow();

        static uint64_t Hash(const Transform &t) {
            const char *ptr = (const char *)(&t.GetMatrix());
            size_t size = sizeof(Matrix4x4);
            uint64_t hash = 14695981039346656037ull;
            while (size > 0) {
                hash ^= *ptr;
                hash *= 1099511628211ull;
                ++ptr;
                --size;
            }
            return hash;
        }

        std::vector<Transform *> hashTable;
        int hashTableOccupancy;
        MemoryArena arena;
    };

    void TransformCache::Insert(Transform *tNew) {
        if (++hashTableOccupancy == hashTable.size() / 2)
            Grow();

        int offset = Hash(*tNew) & (hashTable.size() - 1);
        int step = 1;
        while (true) {
            if (hashTable[offset] == nullptr) {
                hashTable[offset] = tNew;
                return;
            }
            // Advance using quadratic probing.
            offset = (offset + step * step) & (hashTable.size() - 1);
            ++step;
        }
    }

    void TransformCache::Grow() {
        std::vector<Transform *> newTable(2 * hashTable.size());

        // Insert current elements into newTable.
        for (Transform *tEntry : hashTable) {
            if (!tEntry) continue;

            int offset = Hash(*tEntry) & (newTable.size() - 1);
            int step = 1;
            while (true) {
                if (newTable[offset] == nullptr) {
                    newTable[offset] = tEntry;
                    break;
                }
                // Advance using quadratic probing.
                offset = (offset + step * step) & (hashTable.size() - 1);
                ++step;
            }
        }

        std::swap(hashTable, newTable);
    }

    // API Static Data
    static GraphicsState graphicsState;
    static std::unique_ptr<RenderOptions> renderOptions;
    static TransformSet curTransform;
    static uint32_t activeTransformBits = AllTransformsBits;
    static std::map<std::string, TransformSet> namedCoordinateSystems;
    static std::vector<GraphicsState> pushedGraphicsStates;
    static std::vector<TransformSet> pushedTransforms;
    static std::vector<uint32_t> pushedActiveTransformBits;
    static TransformCache transformCache;

#define FOR_ACTIVE_TRANSFORMS(expr)           \
    for (int i = 0; i < MaxTransforms; ++i)   \
        if (activeTransformBits & (1 << i)) { \
            expr                              \
        }


    std::vector<std::shared_ptr<Shape>> MakeShapes(const std::string &name,
                                                   const Transform *object2world,
                                                   const Transform *world2object){
        std::vector<std::shared_ptr<Shape>> shapes;
        std::shared_ptr<Shape> s;
        if (name == "sphere")
            s = CreateSphereShape(object2world, world2object);
        return shapes;
    }

    void pbrtLookAt(float ex, float ey, float ez, float lx, float ly, float lz,
                    float ux, float uy, float uz) {
        Transform lookAt = LookAt(Point3f(ex,ey,ez),Point3f(lx,ly,lz),Vector3f(ux,uy,uz));
        FOR_ACTIVE_TRANSFORMS(curTransform[i] = curTransform[i] * lookAt;);
    }

    std::shared_ptr<Light> MakeLight(const std::string &name,
                                     const Transform &light2world) {
        std::shared_ptr<Light> light;
        if (name == "point")
            light = CreatePointLight(light2world);
        return light;

    }

    void pbrtInit(const Options &opt) {
        PbrtOptions = opt;
        renderOptions.reset(new RenderOptions);
        graphicsState = GraphicsState();
    }

    void pbrtCamera(const std::string &name) {
        renderOptions->CameraName = name;
        renderOptions->CameraToWorld = Inverse(curTransform);
        namedCoordinateSystems["camera"] = renderOptions->CameraToWorld;
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
        std::shared_ptr<Light> lt = MakeLight(name, curTransform[0]);
        renderOptions->lights.push_back(lt);

    }

    void pbrtShape(const std::string &name) {
        std::vector<std::shared_ptr<Primitive>> prims;
        Transform *ObjToWorld = transformCache.Lookup(curTransform[0]);
        Transform *WorldToObj = transformCache.Lookup(Inverse(curTransform[0]));
        std::vector<std::shared_ptr<Shape>> shapes =
                MakeShapes(name, ObjToWorld, WorldToObj);
        if (shapes.empty()) return;
        std::shared_ptr<Material> mtl = graphicsState.GetMaterialForShape();
        prims.reserve(shapes.size());
        for (auto s : shapes) {
            prims.push_back(std::make_shared<GeometricPrimitive>(s, mtl));
        }
    }

    void pbrtTranslate(float dx, float dy, float dz) {
        FOR_ACTIVE_TRANSFORMS(curTransform[i] = curTransform[i] *
                                                Translate(Vector3f(dx, dy, dz));)
    }

    void pbrtCleanup() {

    }


    std::shared_ptr<Sampler> MakeSampler(const std::string &name,
                                         const Film *film) {
        Sampler *sampler = nullptr;
        if (name == "random")
            sampler = CreateRandomSampler();
        return std::shared_ptr<Sampler>(sampler);

    }
    Camera *MakeCamera(const std::string &name, const TransformSet &cam2worldSet, Film *film) {
        Camera *camera = nullptr;
        Transform *cam2world[2] = {
                transformCache.Lookup(cam2worldSet[0]),
                transformCache.Lookup(cam2worldSet[1])
        };
        Transform *c2w = transformCache.Lookup(cam2worldSet[0]);
        camera = CreateOrthographicCamera(*c2w, film);
        return camera;
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

        return integrator;
    }

    Camera *RenderOptions::MakeCamera() const {
        std::unique_ptr<Filter> filter = MakeFilter(FilterName);
        Film *film = MakeFilm(FilmName, std::move(filter));
        Camera *camera = pbrt::MakeCamera(CameraName,CameraToWorld,film);
        return camera;
    }

    Scene *RenderOptions::MakeScene() {
        std::shared_ptr<Primitive> accelerator;
        Scene *scene = new Scene(accelerator,lights);
        primitives.clear();
        lights.clear();
        return scene;
    }

}