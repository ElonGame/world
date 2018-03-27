#include "Tree.h"

#include "worldapi/world/Collector.h"

namespace world {
    Tree::Tree() {

    }

    Tree::~Tree() {

    }

    void Tree::collectWholeObject(ICollector &collector) {
        Object3D mainPart(_trunkMesh);
        collector.addItem(0, mainPart);
    }
}