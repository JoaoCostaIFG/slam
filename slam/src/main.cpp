#include <iostream>
#include "../include/octomap/Octomap.h"

using namespace std;

using namespace octomap;

int main() {
    Vector3 p1 = Vector3(1, 2, 3);

    std::cout << p1 << std::endl;
    Octomap o = Octomap();
    o.updateNode(p1);

    return EXIT_SUCCESS;
}
