#include "ForestLayer.h"

#include "world/core/Chunk.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(IChunkDecorator, ForestLayer, "ForestLayer")

ForestLayer::ForestLayer()
        : _rng(static_cast<u32>(time(NULL))),
          _templateTree(std::make_unique<Tree>()),
          _treeSprite(3, 3, ImageType::RGB) {

    _templateTree->randomize();

    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            _treeSprite.rgb(x, y).setf(0.05, 0.35, 0.0);
        }
    }
}

void ForestLayer::decorate(Chunk &chunk, const ExplorationContext &ctx) {
    // Check resolution
    const double resolution = 0.01;
    const double minres = chunk.getMinResolution();
    const double maxres = chunk.getMaxResolution();

    if (!(resolution >= minres && resolution < maxres)) {
        return;
    }

    const IEnvironment &env = ctx.getEnvironment();

    // Compute area
    vec3d chunkSize = chunk.getSize();
    vec3d chunkOffset = chunk.getPosition3D();
    const double area = chunkSize.x * chunkSize.y;

    // Max tree count
    std::uniform_real_distribution<double> stddistrib(0, 1);
    double maxTreeCountf = _maxDensity * area / 1e6;
    u32 maxTreeCount = static_cast<u32>(floor(maxTreeCountf));

    if (stddistrib(_rng) < maxTreeCountf - maxTreeCount) {
        maxTreeCount++;
    }

    // Generate random points
    std::vector<vec2d> randomPoints;
    randomPoints.reserve(maxTreeCount);

    // TODO use a method that includes a minimal distance between points
    std::uniform_real_distribution<double> xdistrib(0, chunkSize.x);
    std::uniform_real_distribution<double> ydistrib(0, chunkSize.y);

    for (u32 i = 0; i < maxTreeCount; ++i) {
        randomPoints.emplace_back(xdistrib(_rng), ydistrib(_rng));
    }

    // Populate trees
    int remainingTrees = 0;
    Tree *trees = nullptr;

    for (auto &pt : randomPoints) {
        vec3d origin{chunkOffset.x + pt.x, chunkOffset.y + pt.y, 0};
        const double altitude =
            ctx.getEnvironment()
                .findNearestFreePoint(origin, {0, 0, 1}, resolution, ctx)
                .z;

        // skip if altitude is not in this chunk
        if (altitude < chunkOffset.z ||
            altitude > chunkOffset.z + chunkSize.z) {
            continue;
        }

        if (stddistrib(_rng) < getDensityAtAltitude(altitude)) {
            if (remainingTrees <= 0) {
                trees = &chunk.addChild<Tree>();
                trees->setup(*_templateTree);
                trees->setPosition3D(chunkSize / 2.0);
                remainingTrees = 50; // treeGroup->maxTreeCount();
            }

            vec3d pos{pt.x, pt.y, altitude - chunkOffset.z};
            trees->addTree(pos - chunkSize / 2.0);
            --remainingTrees;

            /*ground.paintTexture(
                {chunkOffset.x + pt.x - 2, chunkOffset.y + pt.y - 2}, {4, 4},
                {0, 1}, _treeSprite);*/
        }
    }
}

void ForestLayer::write(WorldFile &wf) const {
    wf.addChild("templateTree",
                dynamic_cast<WorldNode &>(*_templateTree).serialize());
    wf.addDouble("maxDensity", _maxDensity);
}

void ForestLayer::read(const WorldFile &wf) {
    if (wf.hasChild("templateTree")) {
        _templateTree = std::make_unique<Tree>();
        _templateTree->read(wf.readChild("templateTree"));
    }
    wf.readDoubleOpt("maxDensity", _maxDensity);
}

double ForestLayer::getDensityAtAltitude(double altitude) {
    // [,0] -> 0; 100 -> 0.5; 500 -> 0.25; 1000 -> 0.12; 2000 -> 0;
    // TODO utiliser des splines
    if (altitude <= 0) {
        return 0;
    } else if (altitude <= 100) {
        double x = altitude / 100;
        return x * 0.5;
    } else if (altitude <= 500) {
        double x = (altitude - 100) / 400;
        return (1 - x) * 0.5 + x * 0.25;
    } else if (altitude <= 1000) {
        double x = (altitude - 500) / 500;
        return (1 - x) * 0.25 + x * 0.12;
    } else if (altitude <= 2000) {
        double x = (altitude - 1000) / 1000;
        return (1 - x) * 0.12;
    } else
        return 0;
}

} // namespace world
