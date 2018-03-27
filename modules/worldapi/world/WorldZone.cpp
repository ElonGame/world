#include "WorldZone.h"

#include "Chunk.h"

namespace world {

    vec3d IWorldZoneHandler::getAbsoluteOffset() const {
        WorldZone parent(this->clone());
        vec3d offset = parent->getChunk().getOffset();

        while (parent->hasParent()) {
            parent = parent->getParent();
            offset = offset + parent->getChunk().getOffset();
        }

        return offset;
    }

    vec3d IWorldZoneHandler::getRelativeOffset(const WorldZone &other) const {
        // TODO

        // We look for the nearest common ancestor between the two
        // We remember the offset of each with their respective ancestors
        /*
        if (*this == other) {
            return {0, 0, 0};
        }

        vec3d thisOffset;
        vec3d otherOffset;

        WorldZone thisCurrent(*this);
        WorldZone otherCurrent(other);

        std::vector<std::unique_ptr<WorldZone>> thisChain;
        std::vector<std::unique_ptr<WorldZone>> otherChain;

        bool ancestorFound = false;

        while(!ancestorFound && (thisCurrent.hasParent() && otherCurrent.hasParent())) {
            if (thisCurrent.hasParent()) {
                thisCurrent = thisCurrent.getParent();

                if (std::find_if(otherChain.begin(), otherChain.end(),
                                 [] (const std::unique_ptr<WorldZone> & zone) {
                                     return *zone == thisCurrent;
                                 }) != otherChain.end()) {

                    ancestorFound = true;
                }
            }
        }*/
        return {};
    }
}
