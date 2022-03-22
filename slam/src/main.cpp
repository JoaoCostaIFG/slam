#include <iostream>
#include "../include/octomap/Octomap.h"

using namespace std;

using namespace octomap;

int main() {
    Vector3 v1 = Vector3(1, 2, 3);
    Vector3 v2 = Vector3(1, 2, 4);

    cout << v1 << endl << v2 << endl;
    cout << ((v1 == v2) ? "equal" : "not equal") << endl;

    return 0;
}
