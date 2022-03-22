#include "../include/octomap/Octomap.h"

using namespace octomap;

Octomap::Octomap(const unsigned int maxDepth, const float resolution) :
        maxDepth(maxDepth), resolution(resolution) {
    this->rootNode = Ocnode();
}

Octomap::Octomap() : Octomap(DFLT_MAX_DEPTH, DFLT_RESOLUTION) {
}

Ocnode Octomap::updateNode(const Vector3 &location) {
    return Ocnode();
}
