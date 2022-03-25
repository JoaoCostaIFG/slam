#include <algorithm>
#include <bitset>
#include <iostream>

#include "../include/octomap/Octomap.h"

using namespace std;

using namespace octomap;

int main() {
    Vector3 vv = Vector3(1, 2, 3);
    auto it = std::min_element(std::begin(vv), std::end(vv));
    cout << it - std::begin(vv) << endl;

    Octomap o = Octomap();

    for (int x = -20; x < 20; x++) {
        for (int y = -20; y < 20; y++) {
            for (int z = -20; z < 20; z++) {
                Vector3 endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
                // integrate 'occupied' measurement
                OcNode *node = o.setOccupancy(endpoint, 1.0);
            }
        }
    }

    OcNode *n;
    n = o.search(Vector3(1000, 1000, 1000));
    cout << ((n != nullptr) ? "findei\n" : "n encontralhei\n");
    n = o.search(Vector3(0, 0, 0));
    cout << ((n != nullptr) ? "findei\n" : "n encontralhei\n");

    cout << "Size: " << o.getSize() << endl;
    o.writeBinary("rust.bt");

    return EXIT_SUCCESS;
}
