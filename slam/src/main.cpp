#include <bitset>
#include <iostream>
#include <fstream>
#include <ctime>

#include "../include/octomap/Octomap.h"
#include "../include/Scan.h"

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
    //Octomap o = Octomap();

    //o.rayCast(Vector3(), Vector3f(1, 1, 1));
    //o.rayCast(Vector3(), Vector3f(1267.09998, 2835.5, 272.221985));
    //for (auto &point: o.rayCast(Vector3(), Vector3f(67.09998, 35.5, 2.221985))) {
    //    o.setFull(*point);
    //}
    //o.setEmpty(Vector3f(67.09998, 35.5, 2.221985));

    //for (int i = 0; i < 3; ++i) {
    //    clock_t start = clock();
    //    //o.pointcloudUpdate(vector<Vector3f>({Vector3f(67.09998, 35.5, 2.221985)}), Vector3f());
    //    o.pointcloudUpdate(importOff("../datasets/airplane_smaller.off"), Vector3f());
    //    cout << float(clock() - start) / CLOCKS_PER_SEC << endl;
    //}

    //for (const auto& point : importOff("../datasets/airplane_small.off")) {
    //    o.setFull(point);
    //}

    //auto ray = o.rayCast(Vector3<float>(0, 0, 0), Vector3<float>(1.0, 1.0, 1.0));
    //for (auto & it : ray) {
    //    cout << it->toCoord() << endl;
    //    o.setOccupancy(*it, 1.0);
    //}
    //o.setFull(Vector3f(1.0, 1.0, 1.0));

    //cout << "Size: " << o.getSize() << endl;
    //o.writeBinary("rust.bt");

    ifstream ss("../data.json");
    Scan *s = Scan::importJson(ss);
    cout << "Scan: " << *s << endl;
    for (auto beam: s->getBeams())
        cout << *beam << endl;

    return EXIT_SUCCESS;
}
