#include <chrono>
#include <ctime>
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

int main() {
    ZZ test(5000000);
    cout << "This is a little test: " << test << endl;
    return 0;
}