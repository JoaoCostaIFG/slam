//
// Created by joao on 3/22/22.
//

#ifndef SLAM_OCNODE_H
#define SLAM_OCNODE_H

#define OCCUP_UNKOWN 0.5f

class Ocnode {
private:
    Ocnode *children = nullptr;
    float occupancy;
public:
    Ocnode();
    [[nodiscard]] float getOccupancy() const;
};

#endif //SLAM_OCNODE_H
