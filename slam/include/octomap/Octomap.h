#ifndef SLAM_OCTOMAP_H
#define SLAM_OCTOMAP_H

#include "Ocnode.h"
#include "Vector3.h"

#define DFLT_MAX_DEPTH 16
#define DFLT_RESOLUTION 0.01f

namespace octomap {
    class Octomap {
    private:
        const unsigned int maxDepth;
        const float resolution; // in meters

        unsigned int size = 1;

        // number of nodes in the tree (starts with 1 root node)
        Ocnode rootNode;
    public:
        Octomap(unsigned int maxDepth, float resolution);

        Octomap();

        unsigned int getSize() const {
            return this->size;
        }

        Ocnode *updateNode(const Vector3 &location);

        bool writeBinary(std::ostream &os);

        bool writeBinary(const std::string& filename);
    };
}

#endif //SLAM_OCTOMAP_H
