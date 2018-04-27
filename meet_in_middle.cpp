#include <boost/progress.hpp>
#include <iostream>
#include <set>
#include <thread>
#include "util.h"

using namespace std;

class Sum {
public:
    ZZ sum;
    // This is kinda hacky, had to add this to ensure the index of the 'second' doesn't get lost after sorting it
    // However, the memory savings are worth it - (used to be storing all the indices like in the 'Subset' class)
    int index;

    static struct _CompareSum {
        bool operator()(const Sum &left, const Sum &right) {
            return static_cast<bool>(left.sum < right.sum);
        }

        bool operator()(const Sum &left, const ZZ &right) {
            return static_cast<bool>(left.sum < right);
        }

        bool operator()(const ZZ &left, const Sum &right) {
            return static_cast<bool>(left < right.sum);
        }
    } CompareSum;
};

class Subset {
public:
    ZZ sum;
    set<int> indices; // Stores the indices of

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

// This is pretty messy, but it essentially will do one of the following:
// Calculate the subsets of a particular index 'i', or
// Finds the subset indices for a particular index 'i'
void calcSubset(ZZ *a, Sum *possibleSubsets, int i, int n, int offset, int *indexArray, bool calcSum, Subset *subset) {
    ZZ s(0);
    for (int j = 0; j < n; j++)
        if (i & (1 << j)) {
            int index = j + offset;
            if (calcSum) {
                s += a[index];
            } else {
                subset->indices.insert(indexArray[index]);
            }
        }
    if (calcSum) {
        possibleSubsets[i].sum = s;
    }
}

// Calculates all possible subset sums of 'arr'
void calculateSubsets(ZZ *arr, Sum *possibleSubsets, int n, int offset, int *indexArray) {
    for (int i = 0; i < (1 << n); i++) {
        calcSubset(arr, possibleSubsets, i, n, offset, indexArray, true, nullptr);
        possibleSubsets[i].index = i;
    }
}

// Calculates the best subset by finding the sums from 'first' and 'second' that is the smallest
// Precondition: second is sorted, so we can do a binary search on it
void getBestSubset(const ZZ &threshold, Sum *first, Sum *second, int offset, int sizeFirst, int sizeSecond, Subset *min,
                   int *indexArray) {
    boost::progress_display progress(static_cast<unsigned long>(sizeFirst));

    // Rather than updating the subset everytime we find a better sum,
    // we simply store the index of
    int firstMinIndex = 0;
    int secondMinIndex = 0;
    for (int i = offset; i < sizeFirst; i++) {
        ++progress;
        ZZ currX = first[i].sum;
        if (currX <= threshold) {
            // lower_bound() returns the first address
            // which has value greater than or equal to
            // S-first[i].
            int index = lower_bound(second, second + sizeSecond, threshold - currX, Sum::CompareSum) - second;

            // If S-first[i] was not in array second then decrease
            // p by 1
            if (index == sizeSecond) {
                index -= 1;
            }

            ZZ total = second[index].sum + currX;
            if (total < min->sum && total >= threshold) {
                firstMinIndex = i;
                secondMinIndex = second[index].index;
                min->sum = total;
            }
        }
    }
    // Calculate the actual subsets of the subsets using firstMinIndex and secondMinIndex
    calcSubset(nullptr, nullptr, firstMinIndex, 25, 0, indexArray, false, min);
    calcSubset(nullptr, nullptr, secondMinIndex, 25, 25, indexArray, false, min);
}


// Returns the maximum possible sum greater than or equal to S
Subset* solveSubsetSum(ZZ givenArray[], int n, const ZZ &threshold, Sum *first, Sum *second, int *indexArray) {
    // Compute all subset sums of first and second
    // halves
    auto subArrayNow = chrono::system_clock::now();
    time_t timeSub = chrono::system_clock::to_time_t(subArrayNow);
    cout << "Time Stage 1: " << ctime(&timeSub);
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
    cout << "Time Stage 2: " << ctime(&currentTime);
    cout << "Sorting" << endl;
    sort(second, second + sizeSecond, Sum::CompareSum);

    // To keep track of the minimum sum of a subset
    // such that the minimum sum is greater than S
    auto *min = new Subset();
    min->sum = power(to_ZZ(10), 103); // This is greater than the max possible sum there can be

    // Traverse all elements of first and do Binary Search
    // for a pair in second with minimum sum greater than S
    // S = (y[i] + x[i])
    auto traverse = chrono::system_clock::now();
    time_t traverseTime = chrono::system_clock::to_time_t(traverse);
    cout << "Time Stage 3: " << ctime(&traverseTime);
    cout << "Calculating Best Subset" << endl;
    getBestSubset(threshold, first, second, 0, sizeFirst, sizeSecond, min, indexArray);
    return min;
}

int main() {
    bool debug = false;
    Util util(100, 113027942, "middle_lowest_difference.json");

    // Should be divisible by 2.
    // Increasing this number increases the execution time exponentially.
    // The higher this is, the better (theoretically), since it can evaluate more subsets
    // Anecdotally, 54 seems the max I'd want to use
    int meetInMiddleSize = util.n / 2;
    // This number can be anything, simply how many we want to include by default
    int quarterToIncludeSize = 13;


    int bothSize = meetInMiddleSize + quarterToIncludeSize;
    cout << "Making arrays" << endl;
    auto *first = new Sum[(1<<(meetInMiddleSize/2))]; // Of size 2^(n/2)
    auto *second = new Sum[1<<(meetInMiddleSize/2)]; // Of size 2^(n/2)
    cout << "Made arrays" << endl;

    while(util.currentMinimum != util.threshold) {
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
        ZZ *middle = util.convertToZZ(meetInMiddleSize, removedQuarter, meetInMiddleIndices); // The actual values
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
            cout << "Intial Res: [";
            int count = 0;
            for (auto it = res->indices.begin(); it != res->indices.end(); ++it, ++count) {
                cout << *it;
                if (count != res->indices.size() - 1) {
                    cout << ", ";
                }
            }
            cout << "]" << endl;
        }

        if (res->sum < power(to_ZZ(10), 103)) {
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
                util.outputArray(arr, res->indices.size());
            }
            if (total != actualSum) {
                if (!debug) {
                    cout << "res->sum + sum: " << total << endl << "actualSum: " << actualSum << endl;
                }
                cout << "ERROR: first and second sum are not equal" << endl;
            }
            cout << "Logged Diff: " << log(total - util.threshold) / log(10) << endl;
            util.saveIfBetter(static_cast<int>(res->indices.size()), arr, total);

            delete[] arr;
        }

        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        cout << "Time Finished: " << ctime(&currentTime) << endl;

        delete[] removedQuarterIndices;
        delete[] removedQuarter;
        delete[] quarterToInclude;
        delete[] meetInMiddleIndices;
        delete[] middle;
        delete[] quarter;
        delete res;
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
