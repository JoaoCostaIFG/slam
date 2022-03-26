#ifndef SLAM_OCTOMAP_H
#define SLAM_OCTOMAP_H

#include <vector>

#include "OcNode.h"
#include "Vector3.h"

#define DFLT_MAX_DEPTH 16
#define DFLT_RESOLUTION 0.1

namespace octomap {
    class Octomap {
    private:
        const unsigned int depth;
        const double resolution; // in meters

        std::vector<double> stepLookupTable;
        // number of nodes in the tree (starts with 1 root node)
        OcNode *rootNode;
    public:
        Octomap(unsigned int maxDepth, double resolution);

        Octomap();

        ~Octomap();

        [[nodiscard]] unsigned int getSize() const {
            if (this->rootNode == nullptr) return 0;
            return this->rootNode->getChildCount() + 1;
        }

        OcNode *setOccupancy(const OcNodeKey &key, float occ);

        OcNode *setOccupancy(const Vector3<> &location, float occ);

        OcNode *setFull(const OcNodeKey &key) {
            return this->setOccupancy(key, 1.0);
        }

        OcNode *setFull(const Vector3<> &location) {
            return this->setOccupancy(location, 1.0);
        }

        OcNode *setEmpty(const OcNodeKey &key) {
            return this->setOccupancy(key, 0.0);
        }

        OcNode *setEmpty(const Vector3<> &location) {
            return this->setOccupancy(location, 0.0);
        }

        OcNode *updateOccupancy(const OcNodeKey &key, float occ);

        OcNode *updateOccupancy(const Vector3<> &location, float occ);

        OcNode *search(const Vector3<> &location);

        // Algorithm from "A Fast Voxel Traversal Algorithm for Ray Tracing" by John Amanatides & Andrew Woo.
        // Based on DDA ray casting algorithm for 3D.
        std::vector<OcNodeKey> rayCast(const Vector3<> &orig, const Vector3<> &end);

        OcNode* rayCastUpdate(const Vector3<> &orig, const Vector3<> &end, float occ);

        // The binary format is compatible with octoviz
        // See: https://github.com/OctoMap/octomap/tree/devel/octovis
        bool writeBinary(std::ostream &os);

        bool writeBinary(const std::string &filename);
    };
}

#endif //SLAM_OCTOMAP_H
