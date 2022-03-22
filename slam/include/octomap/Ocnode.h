//
// Created by joao on 3/22/22.
//

#ifndef SLAM_OCNODE_H
#define SLAM_OCNODE_H

#include <cassert>
#include <cmath>

#define RIGHT 1
#define FRONT 2
#define UP    4

#define OCCUP_UNKOWN 0.5f

namespace octomap {
    class Ocnode {
    private:
        Ocnode *children = nullptr;
        // TODO store log-odds instead of probability (see funcs bellow)
        float occupancy;

    public:
        Ocnode();

        ~Ocnode();

        Ocnode *getChild(unsigned int pos);

        bool hasChildren();

        [[nodiscard]] float getOccupancy() const;

        void splitNode();

        static double prob2logodds(double prob) {
            return log(prob / (1 - prob));
        }

        static double logodds2prob2(double logodds) {
            return 1.0 - (1.0 / (1.0 + exp(logodds)));
        }
    };
}

#endif //SLAM_OCNODE_H
