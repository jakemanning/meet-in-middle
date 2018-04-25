#include <iostream>
#include <set>
#include "util.h"

using namespace std;

void outputArray(ZZ *array, int size) {
    cout << "[ ";
    for(int i = 0; i < size; ++i) {
        cout << array[i];
        if (i + 1 < size) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

int main() {
    Util util(100, 113027942, "middle_lowest_difference.json");
    int meetInMiddleSize = 50;
    int quarterToIncludeSize = 25;
    int bothSize = meetInMiddleSize + quarterToIncludeSize;

    auto *removedQuarter = util.takeValueSample(bothSize, util.n, util.array); // Takes 75 values from the original 100 (discards 25 values)
    auto *meetInMiddleIndices = util.takeIndexSample(meetInMiddleSize, bothSize); // Takes 50 indices from the 75 possible values
    set<int> quarterSet; // The quarter that we will definitely include
    for (int i = 0; i < bothSize; ++i) {
        quarterSet.insert(i);
    }
    for (int i = 0; i < meetInMiddleSize; ++i) {
        quarterSet.erase(meetInMiddleIndices[i]);
    }
    // quarterSet now contains 25 indices that we will use
    auto quarterToInclude = new int[quarterToIncludeSize];
    int index = 0;
    for (auto it=quarterSet.begin(); it != quarterSet.end(); ++it, ++index) {
        quarterToInclude[index] = *it;
    }
    delete[] quarterToInclude;

    ZZ *quarter = util.convertToZZ(quarterToIncludeSize, removedQuarter, quarterToInclude);
    ZZ *middle = util.convertToZZ(meetInMiddleSize, removedQuarter, meetInMiddleIndices);

    outputArray(quarter, quarterToIncludeSize);
    outputArray(middle, meetInMiddleSize);



    return 0;
}
