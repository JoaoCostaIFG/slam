//
// Created by joao on 3/22/22.
//

#ifndef SLAM_OCNODE_H
#define SLAM_OCNODE_H

#include <cmath>

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

        [[nodiscard]] float getOccupancy() const;

        void splitnode();

        static double prob2logodds(double prob) {
            return log(prob / (1 - prob));
        }

        static double logodds2prob2(double logodds) {
            return 1.0 - (1.0 / (1.0 + exp(logodds)));
        }
    };
}

#endif //SLAM_OCNODE_H
