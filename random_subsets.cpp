#include <iostream>
#include "util.h"

using namespace std;

int main() {
    Util util(100, 113027942, "random_lowest_difference.json");
    ZZ myMin = power(ZZ(10), 110);

    do {
        auto *sample = util.takeRandomSample();
        int subsetOfSample = util.sizeOfFirstSubsetOverThreshold(sample);
        ZZ both = util.sum - util.threshold;
        if (both >= 0 && both < myMin) {
            myMin = both;
            cout << "Current session improved: " << (log(both) / log(10)) << endl;
        }
        util.saveIfBetter(subsetOfSample, sample, util.sum, 0, 0);
        delete[] sample;
    } while(util.sum != util.threshold);
    return 0;
}