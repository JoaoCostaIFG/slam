#include <bitset>
#include <iostream>
#include <fstream>

#include "../include/octomap/Octomap.h"

using namespace std;
using namespace octomap;

// https://segeval.cs.princeton.edu/public/off_format.html
vector<Vector3f> importOff(const string &filename) {
    ifstream f(filename);

    f.ignore(9999, '\n'); // first line should say OFF
    int vertCnt, faceCnt, edgeCnt; // edge cnt is usually ignored
    f >> vertCnt >> faceCnt >> edgeCnt;

    vector<Vector3f> ret;
    float x, y, z;
    for (int i = 0; i < vertCnt; ++i) {
        f >> x >> y >> z;
        ret.emplace_back(x, y, z);
    }

    // we don't care about the object faces
    f.close();
    return ret;
}

int main() {
    Octomap o = Octomap();

    //o.rayCast(Vector3(), Vector3f(1, 1, 1));
    //o.rayCast(Vector3(), Vector3f(1267.09998, 2835.5, 272.221985));
    //for (auto &point: o.rayCast(Vector3(), Vector3f(67.09998, 35.5, 2.221985))) {
    //    o.setFull(*point);
    //}
    //o.setEmpty(Vector3f(67.09998, 35.5, 2.221985));

    o.pointcloudUpdate(vector<Vector3f>({Vector3f(67.09998, 35.5, 2.221985)}), Vector3f());
    //o.pointcloudUpdate(importOff("../datasets/airplane_small.off"), Vector3f());

    //for (const auto& point : importOff("../datasets/airplane_small.off")) {
    //    o.setFull(point);
    //}

    //auto ray = o.rayCast(Vector3<float>(0, 0, 0), Vector3<float>(1.0, 1.0, 1.0));
    //for (auto & it : ray) {
    //    cout << it->toCoord() << endl;
    //    o.setOccupancy(*it, 1.0);
    //}
    //o.setFull(Vector3f(1.0, 1.0, 1.0));

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
