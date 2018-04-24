#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <random>
#include <NTL/RR.h>
#include <NTL/ZZ.h>
#include <string>

using json = nlohmann::json;
using namespace NTL;
using namespace std;

ZZ* fillArray(int n) {
    PrimeSeq seq;
    auto * array = new ZZ[n];
    for (int i = 0; i < n; ++i) {
        RR primeNo(seq.next());
        RR multiplier(1.0/3.0);
        RR res(floor(pow(primeNo, multiplier) * pow(to_RR(10), to_RR(100))));
        array[i] = to_ZZ(res);
    }
    return array;
}
//
//
//
//void calcSubArray(ZZ arr[], ZZ fillMe[], int n, int offset) {
//    for (int i = 0; i <(i<<n); ++i) {
//        ZZ sum(0);
//        for (int j = 0; j < n; ++j) {
//            if (i & (1<<j)) {
//                sum += arr[j + offset];
//            }
//        }
//        fillMe[i] = sum;
//    }
//}
//
//ZZ solveSubsetSum(ZZ givenArray[], int n, ZZ threshold) {
//    cout << givenArray[0] << endl;
//    cout << "Computing first subarray" << endl;
//    ZZ first[2^50];
//    calcSubArray(givenArray, first, n/2, 0);
//
//    cout << "Computing second subarray" << endl;
//    ZZ second[2^50];
//    calcSubArray(givenArray, second, (n - n/2), 0);
//
//    int sizeFirst(1<<(n/2));
//    int sizeSecond(1<<(n - n/2));
//
//    cout << "Sorting" << endl;
//    sort(second, second + sizeSecond);
//    cout << "Finished sorting" << endl;
//
//    ZZ max(0);
//
//    cout << "Traversing all sums?" << endl;
//    for(int i = 0; i < sizeFirst; ++i) {
//        if(first[i] <= threshold) {
//            // lower_bound() returns the first address
//            // which has value greater than or equal to
//            // S-X[i].
//            ZZ *p = lower_bound(second, second + sizeSecond, threshold - first[i]);
//
//            // If S-X[i] was not in array Y then decrease
//            // p by 1
//            if (*p == sizeSecond || second[to_int(*p)] != (threshold - first[i])) {
//                *p = *p - 1;
//            }
//
//            if ((second[to_int(*p)] + first[i]) > max) {
//                cout << max << endl;
//                max = second[to_int(*p)] + first[i];
//            }
//        }
//    }
//    cout << "Finished traversing all sums" << endl;
//
//    return max;
//}

int main() {
    int n = 100;
    ZZ *array = fillArray(n);
    ZZ threshold(power(to_ZZ(10), 94) * 2 * 113027942);

    ifstream in("lowest_difference.json");

    random_device randomDevice;
    mt19937 mt(randomDevice());
    ZZ difference(0);
    ZZ sum(0);
    json input;

    try {
        in >> input;
    } catch(json::parse_error &error) {
        cerr << error.what() << endl;
    }

    ZZ min;
    if (input.find("logged_difference") != input.end()) {
        double diff = input["logged_difference"];
        cout << "Previous best: " << diff << endl;
        min = to_ZZ(floor(pow(to_RR(10), to_RR(diff))));
    }
    else {
        cout << "I was unable to find a previous result. Using default minimum" << endl;
        min = power(to_ZZ(10), 110);
    }

    do {
        uniform_int_distribution<int> arraySizeDist(0, n - 1); // To get sample size
        int sampleSize = arraySizeDist(mt);
        auto *sample = new int[sampleSize];
        uniform_int_distribution<int> sampleDistribution(0, sampleSize - 1);
        uniform_real_distribution<double> realDistribution(0.0, 1.0);

        for (int i = 0; i < n; ++i) {
            if (i < sampleSize) {
                sample[i] = i;
            } else if (i >= sampleSize && (realDistribution(mt) < sampleSize / ((float) (i + 1)))) {
                int replace = sampleDistribution(mt);
                sample[replace] = i;
            }
        }

        sum = to_ZZ(0);
        for (int i = 0; i < sampleSize; ++i) {
            sum += array[sample[i]];
        }

        if (sum >= threshold && sum < min) {
            sort(sample, sample + sampleSize);
            min = sum;
            difference = sum - threshold;
            cout << "Found a better one: " << log(difference)/log(10) << endl;

            json j;
            j["logged_difference"] = log(difference)/log(10);
            for (int i = 0; i < sampleSize; ++i) {
                j["array"].push_back(sample[i]);
            }
            ofstream out("lowest_difference.json");
            out << setw(2) << j << endl;
        }
        delete[] sample;
    } while(true);


//    ZZ test[] = {to_ZZ(3), to_ZZ(34), to_ZZ(4), to_ZZ(12), to_ZZ(5), to_ZZ(2)};
//    int anotherIndex = 6;
//    ZZ testThreshold(10);
//    ZZ result(solveSubsetSum(test, anotherIndex, testThreshold));
//    cout << "Largest value smaller than or equal to given sum is " << result << endl;

    return 0;
}