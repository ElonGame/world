#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include "core/WorldConfig.h"

#include <map>
#include <utility>
#include <functional>

#include "core/WorldZone.h"
#include "flat/IGround.h"
#include "Terrain.h"
#include "ITerrainWorker.h"

namespace world {

class PGround;

/** This class manages an infinite ground with as much details
 * as we want. The access method are quite similar to the ones
 * used on the WorldObjects : you can collect parts of the ground
 * by specifying which part of the world you're wanting to get
 * content from. */
class WORLDAPI_EXPORT Ground : public IGround {
public:
    struct TerrainKey;

    struct Tile;

    Ground(double unitSize = 6000, double minAltitude = -2000,
           double maxAltitude = 4000);

    virtual ~Ground();

    // PARAMETERS
    // TODO constraints
    void setMaxAltitude(double max) { _maxAltitude = max; }

    void setMinAltitude(double min) { _minAltitude = min; }

    void setUnitSize(double unitSize) { _unitSize = unitSize; }

    double getMaxAltitude() const { return _maxAltitude; }

    double getMinAltitude() const { return _minAltitude; }

    double getAltitudeRange() const { return _maxAltitude - _minAltitude; }

    double getUnitSize() const { return _unitSize; }

    // TERRAIN WORKERS
    /** Adds a default worker set to generate heightmaps in the
     * ground. This method is for quick-setup purpose. */
    void setDefaultWorkerSet();

    /** Creates and adds an ITerrainWorker to the Ground. The
     * ITerrainWorkers belong to the core of the generation process.
     * Each part of the ground is generated by a chain of worker,
     * each of which adds one feature after the other to the terrain.
     * */
    template <typename T, typename... Args> T &addWorker(Args &&... args);

    // EXPLORATION
    double observeAltitudeAt(WorldZone zone, double x, double y) override;

    void collectZone(const WorldZone &zone, ICollector &collector);

    void collectZone(const WorldZone &zone, FlatWorld &world,
                     FlatWorldCollector &collector) override;

private:
    PGround *_internal;

    // Param�tres
    /** L'altitude minimum du monde. Le niveau de la mer est fix� � 0. */
    double _minAltitude;
    /** L'altitude maximum du monde. Le niveau de la mer est fix� � 0. */
    double _maxAltitude;
    /** La taille d'un terrain de niveau 0 utilis� pour paver la Map.
Normalement, cette taille est la m�me que celle d'un pixel de la Map. */
    double _unitSize;
    /** Le facteur de subdivision pour les diff�rents niveaux de d�tails.
     * La taille d'un terrain en fonction de son niveau de d�tail est
     * calcul�e ainsi : _unitSize * _factor ^ lod */

    int _factor = 4;
    int _terrainRes = 33;
    /** Texture resolution, relatively to the terrain resolution */
    int _textureRes = 8;
    int _maxLOD = 3;

    int _maxCacheSize = 2000;

    // WORKER
    void addWorkerInternal(ITerrainWorker *worker);

    double observeAltitudeAt(double x, double y, int lvl);

    /** Replace a parent terrain by its children in the collector */
    void replaceTerrain(const TerrainKey &key, ICollector &collector);

    void addTerrain(const TerrainKey &key, ICollector &collector);

    /** Updates the cache, free old used memory if needed (by saving
     * the terrains to files or discard them) */
    void updateCache();


    // ACCESS
    Ground::Tile &provide(const TerrainKey &key);

    void registerAccess(const TerrainKey &key, Tile &tile);

    Terrain &provideTerrain(const TerrainKey &key);

    optional<Mesh &> provideMesh(const TerrainKey &key);

    optional<Terrain &> getCachedTerrain(const TerrainKey &key, int genID);


    // DATA
    /** Gets a unique string id for the given tile in the Ground. */
    std::string getTerrainDataId(const TerrainKey &key) const;

    double getTerrainSize(int level) const;

    double getTerrainResolution(int level) const;

    int getLevelForChunk(const WorldZone &zone) const;

    TerrainKey getParentKey(const TerrainKey &childId) const;


    // GENERATION
    /** Generate the terrain with given coordinates. Assume that:
     * - the terrain wasn't generated yet,
     * - the terrains with higher level at the same place are already
     * generated.*/
    void generateTerrain(const TerrainKey &key);

    friend class PGround;
    friend class GroundContext;
};

template <typename T, typename... Args> T &Ground::addWorker(Args &&... args) {
    T *worker = new T(args...);
    addWorkerInternal(worker);
    return *worker;
};
} // namespace world

#endif // WORLD_TERRAINNODE_H
