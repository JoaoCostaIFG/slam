#include "../include/octomap/Octomap.h"

using namespace octomap;

Octomap::Octomap(const unsigned int maxDepth) : maxDepth(maxDepth) {
    this->rootNode = Ocnode();
}

Octomap::Octomap() : Octomap(DFLT_MAX_DEPTH) {
}
