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
    int minPossibleElementsInSubset = 0;
    int sampleSize = 0;
    ZZ *fillArray();
    ZZ readPreviousMin();
    void saveBest(int sampleSize, const int *betterSample, double logDifference, int quarterSize, int middleSize);
    int smallestSubsetWithSum(ZZ arr[], int n, const ZZ &threshold);
public:
    mt19937 mt{};
    ZZ currentMinimum;
    int n;
    ZZ threshold;
    ZZ *array;
    int *takeRandomSample(); // Random sample of indices
    json &getJson(ifstream &in, json &input) const;
    ZZ minValue(const json &input) const;
    int sizeOfFirstSubsetOverThreshold(const int *sample);
    void saveIfBetter(int size, const int *sample, const ZZ &sum, int quarterSize, int middleSize);
    int *takeIndexSample(int sampleSize, int arraySize);
    ZZ *takeValueSample(int sampleSize, int arraySize, ZZ *original);
    ZZ *convertToZZ(int sampleSize, const ZZ *original, const int *indices) const;
    template <typename T> void outputArray(T *array, int size);

    Util(int n, int id, string fileName);
    ~Util();

    ZZ sum;
    string fileName;
};

Util::Util(int n, int id, string fileName) {
    random_device randomDevice;
    mt19937 mt(randomDevice());

    this->mt = mt;
    this->n = n;
    this->fileName = std::move(fileName);
    this->array = fillArray();
    this->threshold = to_ZZ(power(to_ZZ(10), 94) * 2 * id);
    this->currentMinimum = readPreviousMin();
    this->minPossibleElementsInSubset = smallestSubsetWithSum(this->array, n, this->threshold);
}

ZZ *Util::fillArray() {
    auto *array = new ZZ[this->n];
    ifstream in("M.json");
    json input;
    input = getJson(in, input);

    for (int i = 0; i < n; ++i) {
        string s = input[i];
        array[i] = conv<ZZ>(s.c_str());
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
        cout << error.what() << endl;
    }
    return input;
}

ZZ Util::minValue(const json &input) const {
    ZZ min;
    if (input.find("logged_difference") != input.end()) {
        double diff = input["logged_difference"];
        cout << "Previous best: " << diff << endl;
        if (diff == 0) {
            min = threshold;
        } else {
            min = threshold + to_ZZ(pow(to_RR(10), to_RR(diff)));
        }

    } else {
        cout << "I was unable to find a previous result. Using default minimum" << endl;
        min = power(to_ZZ(10), 250);
    }
    return min;
}

int *Util::takeRandomSample() {
    uniform_int_distribution<int> sizeDist(minPossibleElementsInSubset, n - 1); // To get sample size (30 is the smallest possible sum with n=100)
    int sampleSize = sizeDist(mt);
    this->sampleSize = sampleSize;
    return takeIndexSample(sampleSize, n);
}

int *Util::takeIndexSample(int sampleSize, int arraySize) {
    auto *array = new int[sampleSize]; // An array filled with indices of the original array
    uniform_int_distribution<int> sampleDistribution(0, sampleSize - 1);
    uniform_real_distribution<double> realDistribution(0.0, 1.0); // Number btwn [0.0, 1.0)

    // Uses reservoir sampling to take a random sample consisting of 'sampleSize' from the Prime array
    for (int i = 0; i < arraySize; ++i) {
        if (i < sampleSize) {
            array[i] = i;
        } else if (i >= sampleSize && (realDistribution(mt) < sampleSize / ((float) (i + 1)))) {
            int replace = sampleDistribution(mt);
            array[replace] = i;
        }
    }
    return array;
}

ZZ *Util::takeValueSample(int sampleSize, int arraySize, ZZ *original) {
    int *indices = takeIndexSample(sampleSize, arraySize);
    ZZ *ret = convertToZZ(sampleSize, original, indices);
    delete[] indices;
    return ret;
}

ZZ *Util::convertToZZ(int sampleSize, const ZZ *original, const int *indices) const {
    auto *newValues = new ZZ[sampleSize];
    for (int i = 0; i < sampleSize; ++i) {
        newValues[i] = original[indices[i]];
    }
    return newValues;
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

void Util::saveIfBetter(int size, const int* sample, const ZZ &sum, int quarterSize, int middleSize) {
    if (sum >= threshold && sum < currentMinimum) {
        auto *betterSample = new int[size];
        for (int j = 0; j < size; ++j) {
            betterSample[j] = sample[j];
        }
        sort(betterSample, betterSample + size);
        currentMinimum = sum;
        ZZ difference(sum - threshold);
        double logDifference;
        if (difference == 0) {
            logDifference = 0;
        } else {
            logDifference = log(difference) / log(10);
        }
        cout << "FOUND A BETTER ONE: " << logDifference << endl;
        saveBest(size, betterSample, logDifference, quarterSize, middleSize);
        delete[] betterSample;
    }
}

void Util::saveBest(int sampleSize, const int *betterSample, double logDifference, int quarterSize, int middleSize) {
    json j;
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    j["time"] = ctime(&currentTime);
    j["size"] = sampleSize;
    j["logged_difference"] = logDifference;
    j["quarterSize"] = quarterSize;
    j["middleSize"] = middleSize;
    for (int i = 0; i < sampleSize; ++i) {
            j["array"].push_back(betterSample[i]);
        }
    ofstream out(this->fileName);
    out << setw(2) << j << endl;
}

// Returns length of smallest subarray with sum greater than threshold.
// If there is no subarray with given sum, then returns n+1
int Util::smallestSubsetWithSum(ZZ arr[], int n, const ZZ &threshold) {
    // Initialize current sum and minimum length
    ZZ currSum(0);
    int min_len = n+1;

    // Initialize starting and ending indexes
    int start = 0, end = 0;
    while (end < n)
    {
        // Keep adding array elements while current sum
        // is smaller than threshold
        while (currSum <= threshold && end < n) {
            currSum += arr[end++];
        }

        // If current sum becomes greater than threshold.
        while (currSum > threshold && start < n) {
            // Update minimum length if needed
            if (end - start < min_len)
                min_len = end - start;

            // remove starting elements
            currSum -= arr[start++];
        }
    }
    return min_len;
}

Util::~Util() {
    delete[] array;
    array = nullptr;
}

template<typename T>
void Util::outputArray(T *array, int size)  {
    cout << "[";
    for(int i = 0; i < size; ++i) {
        cout << array[i];
        if (i + 1 < size) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

#endif //RANDOM_SHARED_H
