#include <boost/progress.hpp>
#include <iostream>
#include <set>
#include <thread>
#include "util.h"

using namespace std;

class Subset {
public:
    ZZ sum;
    set<int> indices;

    static struct _CompareSum {
        bool operator()(const Subset &left, const Subset &right) {
            return static_cast<bool>(left.sum < right.sum);
        }

        bool operator()(const Subset &left, const ZZ &right) {
            return static_cast<bool>(left.sum < right);
        }

        bool operator()(const ZZ &left, const Subset &right) {
            return static_cast<bool>(left < right.sum);
        }
    } CompareSum;
};

void getSubset(const ZZ &threshold, const Subset *first, const Subset *second, int offset, int iterSize, int sizeSecond, Subset *min);

void calculateSubsets(ZZ *a, Subset *possibleSubsets, int n, int offset, int *indexArray) {
    for (int i = 0; i < (1 << n); i++) {
        possibleSubsets[i].indices.clear();
        ZZ s(0);
        for (int j = 0; j < n; j++)
            if (i & (1 << j)) {
                int index = j + offset;
                s += a[index];
                possibleSubsets[i].indices.insert(indexArray[index]);
            }
        possibleSubsets[i].sum = s;
    }
}

// Returns the maximum possible sum less or equal to S
Subset* solveSubsetSum(ZZ givenArray[], int n, const ZZ &threshold, Subset *first, Subset *second, int *indexArray) {
    // Compute all subset sums of first and second
    // halves
    auto subArrayNow = chrono::system_clock::now();
    time_t timeSub = chrono::system_clock::to_time_t(subArrayNow);
    cout << "Time before SubArray calculations: " << ctime(&timeSub);
    cout << "Computing First SubArray" << endl;
    thread calcFirst(calculateSubsets, givenArray, first, n / 2, 0, indexArray);
    cout << "Computing Second SubArray" << endl;
    thread calcSecond(calculateSubsets, givenArray, second, n - n / 2, n / 2, indexArray);

    calcFirst.join();
    calcSecond.join();

    int sizeFirst = 1 << (n / 2);
    int sizeSecond = 1 << (n - n / 2);


    // Sort second (we need to do doing binary search in it)
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    cout << "Time before sort: " << ctime(&currentTime);
    cout << "Sorting" << endl;
    sort(second, second + sizeSecond, Subset::CompareSum);

    // To keep track of the minimum sum of a subset
    // such that the minimum sum is greater than S
    auto *min = new Subset();
    min->sum = power(to_ZZ(10), 250);

    // Traverse all elements of first and do Binary Search
    // for a pair in second with minimum sum greater than S
    // S = (y[i] + x[i])
    auto traverse = chrono::system_clock::now();
    time_t traverseTime = chrono::system_clock::to_time_t(traverse);
    cout << "Time before Traversing: " << ctime(&traverseTime);
    cout << "Traversing" << endl;
    getSubset(threshold, first, second, 0, sizeFirst, sizeSecond, min);
    return min;
}

void getSubset(const ZZ &threshold, const Subset *first, const Subset *second, int offset, int iterSize, int sizeSecond, Subset *min) {
    boost::progress_display progress(static_cast<unsigned long>(iterSize));
    int firstPair = 0;
    int secondPair = 0;
    for (int i = offset; i < iterSize; i++) {
        ++progress;
        ZZ currX = first[i].sum;
        if (currX <= threshold) {
            // lower_bound() returns the first address
            // which has value greater than or equal to
            // S-first[i].
            int index = lower_bound(second, second + sizeSecond, threshold - currX, Subset::CompareSum) - second;

            // If S-first[i] was not in array second then decrease
            // p by 1
            if (index == sizeSecond) {
                index -= 1;
            }

            ZZ total = second[index].sum + currX;
            if (total < min->sum && total >= threshold) {
                firstPair = i;
                secondPair = index;
                min->sum = total;
            }
        }
    }
    min->indices.insert(first[firstPair].indices.begin(), first[firstPair].indices.end());
    min->indices.insert(second[secondPair].indices.begin(), second[secondPair].indices.end());
}


int main() {
    bool debug = false;
    Util util(100, 113027942, "middle_lowest_difference.json");
    int meetInMiddleSize = util.n / 2;
    int quarterToIncludeSize = util.n / 5;
    int bothSize = meetInMiddleSize + quarterToIncludeSize;
    auto *first = new Subset[(1<<(meetInMiddleSize/2))]; // Of size 2^(n/2)
    auto *second = new Subset[1<<(meetInMiddleSize/2)]; // Of size 2^(n/2)

    while(util.currentMinimum != util.threshold) {
        cout << "Making arrays" << endl;
        cout << "Made arrays" << endl;
        ZZ threshold = util.threshold;
        int *removedQuarterIndices = util.takeIndexSample(bothSize, util.n);
        ZZ *removedQuarter = util.convertToZZ(bothSize, util.array,
                                              removedQuarterIndices); // Takes n values from the original 100 (discards (100 - n) values)
        int *meetInMiddleIndices = util.takeIndexSample(meetInMiddleSize,
                                                        bothSize); // Takes 50 indices from the 75 possible values

        if (debug) {
            cout << "Removed quarter Indices: ";
            util.outputArray(removedQuarterIndices, bothSize);
            cout << "Removed quarter: ";
            util.outputArray(removedQuarter, bothSize);
            cout << "Meet in middle Indices: ";
            util.outputArray(meetInMiddleIndices, meetInMiddleSize);
        }
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
        for (auto it = quarterSet.begin(); it != quarterSet.end(); ++it, ++index) {
            quarterToInclude[index] = *it;
        }
        if (debug) {
            cout << "Quarter to include Indices: ";
            util.outputArray(quarterToInclude, quarterToIncludeSize);
        }

        ZZ sum(0);
        ZZ *middle = util.convertToZZ(meetInMiddleSize, removedQuarter, meetInMiddleIndices);
        ZZ *quarter = util.convertToZZ(quarterToIncludeSize, removedQuarter, quarterToInclude);
        if (debug) {
            cout << "Middle: ";
            util.outputArray(middle, meetInMiddleSize);
            cout << "Quarter: ";
            util.outputArray(quarter, quarterToIncludeSize);
        }
        for (int i = 0; i < quarterToIncludeSize; ++i) {
            sum += quarter[i];
        }
        threshold -= sum;

        Subset *res = solveSubsetSum(middle, meetInMiddleSize, threshold, first, second, meetInMiddleIndices);

        if (debug) {
            cout << "Intial Res: ";
            for (auto it = res->indices.begin(); it != res->indices.end(); ++it) {
                cout << *it << " ";
            }
            cout << endl;
        }

        if (res->sum != power(to_ZZ(10), 250)) {
            for (int i = 0; i < quarterToIncludeSize; ++i) {
                res->indices.insert(quarterToInclude[i]);
            }
            auto *arr = new int[res->indices.size()];
            ZZ actualSum(0);
            int myIndex = 0;
            for (auto it = res->indices.begin(); it != res->indices.end(); ++it, ++myIndex) {
                int actualIndex = removedQuarterIndices[*it];
                actualSum += util.array[actualIndex];
                arr[myIndex] = actualIndex;
            }
            ZZ total = ZZ(res->sum + sum);
            if (debug) {
                cout << "res->sum + sum: " << total << endl << "actualSum: " << actualSum << endl;
            }
            cout << "Logged Diff: " << log(total - util.threshold) / log(10) << endl;
            util.saveIfBetter(static_cast<int>(res->indices.size()), arr, total);

            delete[] arr;
        }

        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        cout << "Deleting: " << ctime(&currentTime);

        delete[] removedQuarterIndices;
        delete[] removedQuarter;
        delete[] quarterToInclude;
        delete[] meetInMiddleIndices;
        delete[] middle;
        delete[] quarter;
        delete res;
        cout << "Deleted" << endl << endl;
    }
    cout << "Finished! " << endl << "Best: ";
    ifstream in(util.fileName);
    json input;
    input = util.getJson(in, input);
    int size = input["size"];
    auto *indices = new int[size];
    for(int i = 0; i < size; ++i) {
        int s = input["array"][i];
        indices[i] = s;
    }
    util.outputArray(indices, size);

    return 0;
}
