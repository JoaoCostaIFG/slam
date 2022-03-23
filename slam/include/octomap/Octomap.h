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

        Vector3 treeCenter;
        std::vector<double> stepLookupTable;

        // number of nodes in the tree (starts with 1 root node)
        unsigned int size = 1;
        OcNode rootNode;
    public:
        Octomap(unsigned int maxDepth, double resolution);

        Octomap();

        [[nodiscard]] unsigned int getSize() const {
            return this->size;
        }

        OcNode *updateNode(const Vector3 &location);

        bool writeBinary(std::ostream &os);

        bool writeBinary(const std::string& filename);
    };
}

#endif //SLAM_OCTOMAP_H
