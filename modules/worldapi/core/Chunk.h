#pragma once

#include "core/WorldConfig.h"

#include <tuple>
#include <memory>
#include <iterator>
#include <functional>

#include "math/BoundingBox.h"
#include "math/MathsHelper.h"

#include "ICollector.h"
#include "WorldObject.h"

namespace world {

	class PrivateChunk;

    /**  */
	class WORLDAPI_EXPORT Chunk {
	public:
		typedef int ObjectKey;

		Chunk(const vec3d &offset, const vec3d &size);

		Chunk(const Chunk &chunk) = delete;

		virtual ~Chunk();

		void setDetailSizeBounds(double min, double max);

		double getMinDetailSize() const { return _minDetailSize; }

		double getMaxDetailSize() const { return _maxDetailSize; }

		const vec3d &getSize() const { return _size; }

		const vec3d &getOffset() const { return _offset; }

		void addObject(WorldObject *object);

		template<typename T, typename... Args>
		T &createObject(Args... args) {
			addObjectInternal(new T(args...));
		}

		void collectWholeChunk(ICollector &collector);

        void collectPart(const BoundingBox &bbox, double maxDetailSize, ICollector &collector);

	private:
		double _minDetailSize = 0;
		double _maxDetailSize = 1e100;
		vec3d _offset;
		vec3d _size;

		PrivateChunk *_internal;

		void addObjectInternal(WorldObject *object);
	};
}