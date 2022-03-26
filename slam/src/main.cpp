#include <bitset>
#include <iostream>

#include "../include/octomap/Octomap.h"

using namespace std;

using namespace octomap;

int main() {
    Octomap o = Octomap();

    auto ray = o.rayCast(Vector3<float>(0, 0, 0), Vector3<float>(1.0, 1.0, 1.0));
    for (auto & it : ray) {
        cout << it.toCoord() << endl;
        o.setOccupancy(it, 1.0);
    }
    o.setFull(Vector3f(1.0, 1.0, 1.0));

    //int base = 0;
    //for (int x = base + -20; x < base + 20; x++) {
    //    for (int y = base + -20; y < base + 20; y++) {
    //        for (int z = base + -20; z < base + 20; z++) {
    //            Vector3 endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
    //            // integrate 'occupied' measurement
    //            o.setFull(endpoint);
    //        }
    //    }
    //}

    //base = -20;
    //for (int x = base + -20; x < base + 20; x++) {
    //    for (int y = base + -20; y < base + 20; y++) {
    //        for (int z = base + -20; z < base + 20; z++) {
    //            Vector3 endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
    //            // integrate 'occupied' measurement
    //            o.setFull(endpoint);
    //        }
    //    }
    //}

    //base = 20;
    //for (int x = base + -20; x < base + 20; x++) {
    //    for (int y = base + -20; y < base + 20; y++) {
    //        for (int z = base + -20; z < base + 20; z++) {
    //            Vector3 endpoint((float) x * 0.05f, (float) y * 0.05f, (float) z * 0.05f);
    //            // integrate 'occupied' measurement
    //            o.setFull(endpoint);
    //        }
    //    }
    //}

    //OcNode *n;
    //n = o.search(Vector3<>(1000, 1000, 1000));
    //cout << ((n != nullptr) ? "findei\n" : "n encontralhei\n");
    //n = o.search(Vector3<>(0, 0, 0));
    //cout << ((n != nullptr) ? "findei\n" : "n encontralhei\n");

    cout << "Size: " << o.getSize() << endl;
    o.writeBinary("rust.bt");

    return EXIT_SUCCESS;
}
