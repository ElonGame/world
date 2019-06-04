#pragma once

#include "world/core/WorldConfig.h"

#include <memory>
#include <vector>

#include "world/core/WorldTypes.h"
#include "Object3D.h"
#include "Material.h"
#include "Image.h"

namespace world {

class PScene;

class WORLDAPI_EXPORT Scene {
public:
    Scene();

    virtual ~Scene();

    /** Add all the content from the other scene in this scene. */
    void addAll(const Scene &other);

    void addObject(const Object3D &object);

    void getObjects(std::vector<Object3D *> &output) const;

    std::vector<Object3D *> getObjects() const;

    void addMaterial(const Material &material);

    void addMaterial(const std::shared_ptr<Material> &material);

    /** Insert all the materials from this scene in the output vector.*/
    void getMaterials(std::vector<std::shared_ptr<Material>> &output) const;

    std::vector<std::shared_ptr<Material>> getMaterials() const;

    /** Add a texture to the scene. The given image is copied into
     * the scene. */
    void addTexture(const std::string &id, const Image &image);

    optional<const Image &> getTexture(const std::string &id) const;

private:
    PScene *_internal;

    void addObjectInternal(Object3D *object);
};
} // namespace world