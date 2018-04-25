// C++ program to demonstrate working of Meet in the
// Middle algorithm for maximum subset sum problem.
#include <climits>
#include <iostream>
#include <NTL/ZZ.h>
#include <set>

using namespace NTL;
using namespace std;
typedef long long int ll;

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

Subset first[16], second[16];

void calculateSubsets(ZZ *a, Subset *possibleSubsets, int n, int offset) {
    for (int i = 0; i < (1 << n); i++) {
        ZZ s(0);
        for (int j = 0; j < n; j++)
            if (i & (1 << j)) {
                int index = j + offset;
                s += a[index];
                possibleSubsets[i].indices.insert(index);
            }
        possibleSubsets[i].sum = s;
    }
}

// Returns the maximum possible sum less or equal to S
Subset solveSubsetSum(ZZ givenArray[], int n, const ZZ &threshold) {
    // Compute all subset sums of first and second
    // halves
    cout << "Computing First SubArray" << endl;
    calculateSubsets(givenArray, first, n / 2, 0);
    cout << "Computing Second SubArray" << endl;
    calculateSubsets(givenArray, second, n - n / 2, n / 2);

    int size_X = 1 << (n / 2);
    int size_Y = 1 << (n - n / 2);

    cout << "Sorting" << endl;
    // Sort second (we need to do doing binary search in it)
    sort(second, second + size_Y, Subset::CompareSum);

    // To keep track of the minimum sum of a subset
    // such that the minimum sum is greater than S
    Subset min;
    min.sum = power(to_ZZ(10), 250);

    // Traverse all elements of first and do Binary Search
    // for a pair in second with maximum sum less than S.
    // S = (y[i] + x[i])
    cout << "Traversing" << endl;
    for (int i = 0; i < size_X; i++) {
        ZZ currX = first[i].sum;
        if (currX <= threshold) {
            // lower_bound() returns the first address
            // which has value greater than or equal to
            // S-first[i].
            int index = lower_bound(second, second + size_Y, threshold - currX, Subset::CompareSum) - second;

            // If S-first[i] was not in array second then decrease
            // p by 1
            if (index == size_Y) {
                index -= 1;
            }

            ZZ total = second[index].sum + currX;
            if (total < min.sum && total >= threshold) {
                second[index].indices.insert(first[i].indices.begin(), first[i].indices.end());
                min.sum = total;
                min.indices = second[index].indices;
            }
        }
    }
    return min;
}

int main() {
    ZZ a[] = {ZZ(3), ZZ(34), ZZ(4), ZZ(12), ZZ(5), ZZ(2), ZZ(19), ZZ(9)};
    int n = sizeof(a) / sizeof(a[0]);

    ZZ S = ZZ(25);
    Subset res = solveSubsetSum(a, n, S);
    cout << "Largest value smaller than or equal to given sum is " << res.sum << endl;
    cout << "Indices: [";
    for (auto it = res.indices.begin(); it != res.indices.end();) {
        cout << *it;

        if (++it != res.indices.end()) {
            cout << ", ";
        }
    }
    cout << "]" << endl;

    cout << "Values: ";
    for (auto it = res.indices.begin(); it != res.indices.end();) {
        cout << a[*it];

        if (++it != res.indices.end()) {
            cout << " + ";
        }
    }


    return 0;
}
