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
        min = threshold + to_ZZ(pow(to_RR(10), to_RR(diff)));
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

    return 0;
}