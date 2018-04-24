#ifndef RANDOM_SHARED_H
#define RANDOM_SHARED_H

#include <chrono>
#include <ctime>
#include <fstream>
#include <nlohmann/json.hpp>
#include <NTL/RR.h>
#include <NTL/ZZ.h>
#include <string>
#include <random>

using json = nlohmann::json;
using namespace NTL;
using namespace std;

class Util {
private:
    int n;
    ZZ *array;
    ZZ threshold;
    string fileName;
    mt19937 mt{};
    ZZ min;
    ZZ sum;
    int sampleSize = 0;
    ZZ *fillArray();
    ZZ readPreviousMin();
    void saveBest(int sampleSize, const int *betterSample, const ZZ &difference);
public:
    int *takeRandomSample(); // Random sample of indices
    json &getJson(ifstream &in, json &input) const;
    ZZ minValue(const json &input) const;
    int sizeOfFirstSubsetOverThreshold(const int *sample);
    void saveIfBetter(int size, const int *sample);

    Util(int n, int id, string fileName);
};

Util::Util(int n, int id, string fileName) {
    random_device randomDevice;
    mt19937 mt(randomDevice());

    this->mt = mt;
    this->n = n;
    this->fileName = std::move(fileName);

    this->array = fillArray();
    this->threshold = to_ZZ(power(to_ZZ(10), 94) * 2 * id);
    this->min = readPreviousMin();
}

ZZ *Util::fillArray() {
    PrimeSeq seq;
    auto *array = new ZZ[this->n];
    for (int i = 0; i < this->n; ++i) {
        RR primeNo(seq.next());
        RR multiplier(1.0 / 3.0);
        RR res(floor(pow(primeNo, multiplier) * pow(to_RR(10), to_RR(100))));
        array[i] = to_ZZ(res);
    }
    return array;
}

ZZ Util::readPreviousMin() {
    ifstream in(this->fileName);
    json input; // Empty json
    input = getJson(in, input);
    return minValue(input);
}

json &Util::getJson(ifstream &in, json &input) const {
    try {
        in >> input;
    } catch (json::parse_error &error) {
        cerr << error.what() << endl;
    }
    return input;
}

ZZ Util::minValue(const json &input) const {
    ZZ min;
    if (input.find("logged_difference") != input.end()) {
        double diff = input["logged_difference"];
        cout << "Previous best: " << diff << endl;
        min = threshold + to_ZZ(pow(to_RR(10), to_RR(diff)));
    } else {
        cout << "I was unable to find a previous result. Using default minimum" << endl;
        min = power(to_ZZ(10), 110);
    }
    return min;
}

int *Util::takeRandomSample() {
    uniform_int_distribution<int> sizeDist(30, n - 1); // To get sample size (30 is the smallest possible sum with n=100)
    int sampleSize = sizeDist(mt);
    auto *sample = new int[sampleSize]; // An array filled with indices of the original array
    uniform_int_distribution<int> sampleDistribution(0, sampleSize - 1);
    uniform_real_distribution<double> realDistribution(0.0, 1.0); // Number btwn [0.0, 1.0)

    // Uses reservoir sampling to take a random sample consisting of 'sampleSize' from the Prime array
    for (int i = 0; i < n; ++i) {
        if (i < sampleSize) {
            sample[i] = i;
        } else if (i >= sampleSize && (realDistribution(mt) < sampleSize / ((float) (i + 1)))) {
            int replace = sampleDistribution(mt);
            sample[replace] = i;
        }
    }
    this->sampleSize = sampleSize;
    return sample;
}

int Util::sizeOfFirstSubsetOverThreshold(const int *sample) {
    int betterSampleSize = this->sampleSize;
    ZZ sum(0);
    for (int i = 0; i < this->sampleSize; ++i) {
        sum += array[sample[i]];
        if (sum >= threshold) {
            betterSampleSize = i + 1; // Save the first point at which the sum is greater than the threshold
            break;
        }
    }
    this->sum = sum;
    return betterSampleSize;
}

void Util::saveIfBetter(int size, const int* sample) {
    if (sum >= threshold && sum < min) {
        auto *betterSample = new int[size];
        for (int j = 0; j < size; ++j) {
            betterSample[j] = sample[j];
        }
        sort(betterSample, betterSample + size);
        min = sum;
        ZZ difference(sum - threshold);
        cout << "Found a better one: " << log(difference) / log(10) << endl;
        saveBest(size, betterSample, difference);
    }
}

void Util::saveBest(int sampleSize, const int *betterSample, const ZZ &difference) {
    json j;
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    j["time"] = ctime(&currentTime);
    j["logged_difference"] = log(difference) / log(10);
    for (int i = 0; i < sampleSize; ++i) {
            j["array"].push_back(betterSample[i]);
        }
    ofstream out(this->fileName);
    out << setw(2) << j << endl;
    delete[] betterSample;

}

#endif //RANDOM_SHARED_H
