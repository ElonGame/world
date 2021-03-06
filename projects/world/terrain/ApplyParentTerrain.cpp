#include "ApplyParentTerrain.h"

#include "TerrainOps.h"

namespace world {

ApplyParentTerrain::ApplyParentTerrain()
        : _childRate(0.1), _parentOverflow(0.01) {}

void ApplyParentTerrain::processTerrain(world::Terrain &terrain) {
    // Does nothing
}

void ApplyParentTerrain::processTile(ITileContext &context) {
    // Get parent. Generate it if needed
    TerrainTile &tile = context.getTile();
    Terrain &child = tile.terrain();
    TileCoordinates coords = context.getCoords();
    int lvl = coords._lod;

    if (lvl == 0)
        return;

    TileCoordinates parentCoords = context.getParentCoords();
    TerrainElement *parentElem;

    if (!_storage.tryGet(parentCoords, &parentElem))
        return;

    Terrain &parent = parentElem->_terrain;

    int res = child.getResolution();

    // Useful variables
    auto pbbox = parent.getBoundingBox();
    auto cbbox = child.getBoundingBox();
    const vec3d offset = cbbox.getLowerBound() - pbbox.getLowerBound();

    const double parentWidth = pbbox.getDimensions().x;
    const double ratio = cbbox.getDimensions().x / parentWidth;
    const double oX = offset.x / parentWidth;
    const double oY = offset.y / parentWidth;

    const double childProp = getContribution(lvl, ratio);
    const double parentProp = 1 + _parentOverflow - childProp;

    arma::mat bufferParent(res, res);

    for (int x = 0; x < res; x++) {
        for (int y = 0; y < res; y++) {
            bufferParent(x, y) = parent.getInterpolatedHeight(
                                     oX + ((double)x / (res - 1)) * ratio,
                                     oY + ((double)y / (res - 1)) * ratio,
                                     Interpolation::LINEAR) *
                                 parentProp;

            // to unapply :
            // * (unapply ? -1. : 1.)
        }
    }

    TerrainOps::multiply(child, childProp);
    TerrainOps::applyOffset(child, bufferParent);

    // to unapply
    // TerrainOps::applyOffset(child, bufferParent);
    // TerrainOps::multiply(child, 1. / childProp);
    _storage.set(coords, child);
}

double ApplyParentTerrain::getContribution(int parentCount, double ratio) {
    return parentCount == 0 ? 1 : 0.1 * powi(ratio, parentCount - 1);
}
} // namespace world