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

void calcSubArray(ZZ arr[], ZZ fillMe[], int n, int offset) {
    for (int i = 0; i <(i<<n); ++i) {
        ZZ sum(0);
        for (int j = 0; j < n; ++j) {
            if (i & (1<<j)) {
                sum += arr[j + offset];
            }
        }
        fillMe[i] = sum;
    }
}

ZZ solveSubsetSum(ZZ givenArray[], int n, ZZ threshold) {
    cout << givenArray[0] << endl;
    cout << "Computing first subarray" << endl;
    ZZ first[2^50];
    calcSubArray(givenArray, first, n/2, 0);

    cout << "Computing second subarray" << endl;
    ZZ second[2^50];
    calcSubArray(givenArray, second, (n - n/2), 0);

    int sizeFirst(1<<(n/2));
    int sizeSecond(1<<(n - n/2));

    cout << "Sorting" << endl;
    sort(second, second + sizeSecond);
    cout << "Finished sorting" << endl;

    ZZ max(0);

    cout << "Traversing all sums?" << endl;
    for(int i = 0; i < sizeFirst; ++i) {
        if(first[i] <= threshold) {
            // lower_bound() returns the first address
            // which has value greater than or equal to
            // S-X[i].
            ZZ *p = lower_bound(second, second + sizeSecond, threshold - first[i]);

            // If S-X[i] was not in array Y then decrease
            // p by 1
            if (*p == sizeSecond || second[to_int(*p)] != (threshold - first[i])) {
                *p = *p - 1;
            }

            if ((second[to_int(*p)] + first[i]) > max) {
                cout << max << endl;
                max = second[to_int(*p)] + first[i];
            }
        }
    }
    cout << "Finished traversing all sums" << endl;

    return max;
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

    return 0;
}
