// C++ program to demonstrate working of Meet in the
// Middle algorithm for maximum subset sum problem.
#include <climits>
#include <iostream>
#include <set>

using namespace std;
typedef long long int ll;

class Subset {

public:
    ll sum;
    set<ll> indices;
    static struct _CompareSum {
        bool operator() (const Subset & left, const Subset & right)
        {
            return left.sum < right.sum ;
        }
        bool operator() (const Subset & left, ll right)
        {
            return left.sum < right;
        }
        bool operator() (ll left, const Subset & right)
        {
            return left < right.sum;
        }
    } CompareSum;
};

Subset X[2000005], Y[2000005];

// Find all possible sum of elements of a[] and store
// in x[]
void calcsubarray(ll a[], Subset x[], int n, int c)
{
    for (int i=0; i<(1<<n); i++)
    {
        ll s = 0;
        for (int j=0; j<n; j++)
            if (i & (1<<j)) {
                int index = j + c;
                s += a[index];
                x[i].indices.insert(index);
            }
        x[i].sum = s;
    }
}

// Returns the maximum possible sum less or equal to S
Subset solveSubsetSum(ll a[], int n, ll S)
{
    // Compute all subset sums of first and second
    // halves
    calcsubarray(a, X, n/2, 0);
    calcsubarray(a, Y, n-n/2, n/2);

    int size_X = 1<<(n/2);
    int size_Y = 1<<(n-n/2);

    // Sort Y (we need to do doing binary search in it)
    sort(Y, Y+size_Y, Subset::CompareSum);

    // To keep track of the minimum sum of a subset
    // such that the minimum sum is greater than S
    Subset min;
    min.sum = LLONG_MAX;

    // Traverse all elements of X and do Binary Search
    // for a pair in Y with maximum sum less than S.
    // S = (y[i] + x[i])
    for (int i=0; i<size_X; i++)
    {
        ll currX = X[i].sum;
        if (currX <= S)
        {
            // lower_bound() returns the first address
            // which has value greater than or equal to
            // S-X[i].
            int index = lower_bound(Y, Y + size_Y, S - currX, Subset::CompareSum) - Y;

            // If S-X[i] was not in array Y then decrease
            // p by 1
            if (index == size_Y) {
                index -= 1;
            } else if(Y[index].sum != (S - currX)) {
//                if (index + 1 != size_Y)
//                    index += 1;
            }

            ll total = Y[index].sum + currX;
            if (total < min.sum && total >= S) {
                Y[index].indices.insert(X[i].indices.begin(), X[i].indices.end());
                min.sum = total;
                min.indices = Y[index].indices;
            }
        }
    }
    return min;
}

// Returns the maximum possible sum less or equal to S
//ll original(ll a[], int n, ll S)
//{
//    // Compute all subset sums of first and second
//    // halves
//    calcsubarray(a, X, n/2, 0);
//    calcsubarray(a, Y, n-n/2, n/2);
//
//    int size_X = 1<<(n/2);
//    int size_Y = 1<<(n-n/2);
//
//    // Sort Y (we need to do doing binary search in it)
//    sort(Y, Y+size_Y);
//
//    // To keep track of the maximum sum of a subset
//    // such that the maximum sum is less than S
//    ll max = 0;
//
//    // Traverse all elements of X and do Binary Search
//    // for a pair in Y with maximum sum less than S.
//    for (int i=0; i<size_X; i++)
//    {
//        ll currX = X[i];
//        if (currX <= S)
//        {
//            // lower_bound() returns the first address
//            // which has value greater than or equal to
//            // S-X[i].
//            unsigned long p = lower_bound(Y, Y + size_Y, S - currX) - Y;
//
//            // If S-X[i] was not in array Y then decrease
//            // p by 1
//            if (p == size_Y || Y[p] != (S-currX))
//                p--;
//            if ((Y[p]+currX) > max)
//                max = Y[p]+currX;
//        }
//    }
//    return max;
//}

// Driver code
int main()
{
    ll a[] = {3, 34, 4, 12, 5, 2};
    int n=sizeof(a)/sizeof(a[0]);

    ll S = 61;
    Subset res = solveSubsetSum(a, n, S);
    cout << "Largest value smaller than or equal to given sum is " << res.sum << endl;
    cout << "Indices: [";
    for (auto it = res.indices.begin(); it != res.indices.end();) {
        cout << *it;

        if(++it != res.indices.end()) {
            cout << ", ";
        }
    }
    cout << "]" << endl;

    cout << "Values: [";
    for (auto it = res.indices.begin(); it != res.indices.end();) {
        cout << a[*it];

        if(++it != res.indices.end()) {
            cout << " + ";
        }
    }
    cout << "]";


    return 0;
}
