#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <map>
#include <utility>

#include "../Image.h"
#include "../maths/perlin.h"
#include "TerrainTexmapBuilder.h"
#include "terrain.h"

#define DEFAULT_TERRAIN_SIZE 257

class WORLDAPI_EXPORT TerrainGenerator {
public:
	TerrainGenerator(int size = DEFAULT_TERRAIN_SIZE);
	virtual ~TerrainGenerator();

	virtual Terrain * generate() = 0;

	// ACCESSEURS
	void setSize(int size);

	img::Image generateTexture(const Terrain & terrain, const TerrainTexmapBuilder & builder, const arma::Mat<double> & randomArray) const;
	img::Image generateTexture(const Terrain & terrain, const arma::Cube<double> & map, const arma::Mat<double> & randomArray) const;

	void generateSubdivisions(Terrain & terrain, int subdivideFactor, int subdivisionsCount);
	virtual void generateSubdivisionLevel(Terrain & terrain, int subdivideFactor);

	virtual TerrainGenerator * clone() const = 0;
protected :
	/// Indique la taille du terrain � g�n�rer
	int _size;
	/// Indique si la texture doit �tre g�n�r�e automatiquement avec le terrain
	bool _generateTexture;

	// ------

	virtual void generateSubdivision(Terrain & terrain, int x, int y) = 0;
};

class WORLDAPI_EXPORT PerlinTerrainGenerator : public TerrainGenerator {
public :
	PerlinTerrainGenerator(int size = DEFAULT_TERRAIN_SIZE, int offset = 0, int octaveCount = 5, float frequency = 8, float persistence = 0.5);
	virtual ~PerlinTerrainGenerator();

	virtual Terrain * generate();

	virtual void generateSubdivisionLevel(Terrain & terrain, int subdivideFactor);

	TerrainGenerator * clone() const override;
protected :
	virtual void generateSubdivision(Terrain & terrain, int x, int y);
private :
	Perlin _perlin;
	mutable std::map<std::pair<int, int>, arma::Mat<double>> _buffer;

	int _offset;
	int _octaveCount;
	float _frequency;
	float _persistence;
	double _subdivNoiseRatio = 0.1;

	// -------
	arma::mat & getBuf(int x, int y) const;

	template <typename... Args>
	arma::Mat<double> & createInBuf(int x, int y, Args&&... args) const {
		std::pair<int, int> index = std::make_pair(x, y);
		_buffer[index] = arma::Mat<double>(args...);
		return _buffer[index];
    }

	void adaptFromBuffer(Terrain & terrain, int xsub, int ysub) const;
};
