#include <bitset>
#include <iostream>

#include "../include/octomap/Octomap.h"

using namespace std;

using namespace octomap;

int main() {
    Octomap o = Octomap();

    for (int x = -20; x < 20; x++) {
        for (int y = -20; y < 20; y++) {
            for (int z = -20; z < 20; z++) {
                Vector3 endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
                // integrate 'occupied' measurement
                OcNode *node = o.setOccupancy(endpoint, 1.0);
                cout << node->getLogOdds() << " " << node->getOccupancy() << endl;
            }
        }
    }

    cout << "Size: " << o.getSize() << endl;
    o.writeBinary("rust.bt");

    return EXIT_SUCCESS;
}
