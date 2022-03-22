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
        Ocnode rootNode;
    public:
        Octomap(unsigned int maxDepth, float resolution);

        Octomap();

        Ocnode updateNode(const Vector3 &location);
    };
}

#endif //SLAM_OCTOMAP_H
