#include "FlatWorld.h"

#include "SimpleTreeDecorator.h"
#include "FlatWorldCollector.h"

namespace world {

	class PrivateFlatWorld {
	public:
		PrivateFlatWorld() {}

		std::vector<std::unique_ptr<FlatWorldDecorator>> _chunkDecorators;
	};

	FlatWorld* FlatWorld::createDemoFlatWorld() {
		FlatWorld *world = new FlatWorld();

		world->addFlatWorldDecorator(new SimpleTreeDecorator(1));

		return world;
	}

	FlatWorld::FlatWorld() : _internal(new PrivateFlatWorld()) {

	}

	FlatWorld::~FlatWorld() {
		delete _internal;
	}

	Ground &FlatWorld::ground() {
		return _ground;
	}

	void FlatWorld::addFlatWorldDecorator(FlatWorldDecorator *decorator) {
		_internal->_chunkDecorators.emplace_back(decorator);
	}

	void FlatWorld::collect(const WorldZone &zone, FlatWorldCollector &collector) {
		World::collect(zone, collector);

		ground().collectZone(collector, *this, zone);
	}

	void FlatWorld::onFirstExploration(WorldZone &chunk) {
		World::onFirstExploration(chunk);

		for (auto &decorator : _internal->_chunkDecorators) {
			decorator->decorate(*this, chunk);
		}
	}
}
