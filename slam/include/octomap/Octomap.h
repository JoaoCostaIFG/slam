#ifndef SLAM_OCTOMAP_H
#define SLAM_OCTOMAP_H

#include "Ocnode.h"

#define DFLT_MAX_DEPTH 16

class Octomap {
private:
    const unsigned int maxDepth;
    Ocnode rootNode;
public:
    Octomap(unsigned int maxDepth);
    Octomap();
};


#endif //SLAM_OCTOMAP_H
