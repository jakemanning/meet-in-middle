#include <iostream>
#include "util.h"

using namespace std;

int main() {
    Util util(100, 113027942, "middle_lowest_difference.json");

    do {
        auto *sample = util.takeRandomSample();
        int subsetOfSample = util.sizeOfFirstSubsetOverThreshold(sample);
        util.saveIfBetter(subsetOfSample, sample);
        delete[] sample;
    } while(true);
    return 0;
}