#include <iostream>
#include <bitset>
#include <random>
#include <cmath>
#include <map>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>

// Include the math_utils namespace and functions provided here.
#include "math_utils.h"

const int iterations = 10000000;
const uint64_t DEFAULT_PRIME = 13166748625691186689ULL;
const uint64_t DEFAULT_GENERATOR = 3180491373ULL;

void run_avalanche_tests(uint64_t P, uint64_t G) {
    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::map<int, int> histogram;
    int sumBitChanges = 0;
    int minBitChanges = 64, maxBitChanges = 0;

    for (int i = 1; i <= iterations; i++) {
        uint64_t state = gen();
        uint64_t modified_state = state ^ (1ULL << (gen() % 64));

        uint64_t state_prime = (state * G) % P;
        uint64_t modified_state_prime = (modified_state * G) % P;

        int bitsChanged = math_utils::hamming_distance(state_prime, modified_state_prime);
        histogram[bitsChanged]++;

        if (bitsChanged < minBitChanges) {
            minBitChanges = bitsChanged;
        }
        if (bitsChanged > maxBitChanges) {
            maxBitChanges = bitsChanged;
        }

        sumBitChanges += bitsChanged;
    }

    double average = static_cast<double>(sumBitChanges) / iterations;
    int mode = std::max_element(histogram.begin(), histogram.end(),
                    [](const std::pair<int, int>& a, const std::pair<int, int>& b)->bool{
                        return a.second < b.second;
                    })->first;

    double sumOfSquaredDifferences = 0;
    for (auto &entry : histogram) {
        sumOfSquaredDifferences += (entry.first - average) * (entry.first - average) * entry.second;
    }
    double stdev = std::sqrt(sumOfSquaredDifferences / iterations);

    std::cout << "\nHistogram:" << std::endl;
    for (auto &entry : histogram) {
        std::cout << std::setw(2) << entry.first << " bits: ";
        int barLength = entry.second / (iterations/500);
        for (int i = 0; i < barLength; i++) std::cout << "#";
        if (barLength > 0) std::cout << " ";
        std::cout << "(" << entry.second << ")" << std::endl;
    }

    std::cout << "\nMin bits changed: " << minBitChanges << std::endl;
    std::cout << "Max bits changed: " << maxBitChanges << std::endl;
    std::cout << "Mode of bits changed: " << mode << std::endl;

    std::cout << "\nAverage percentage of bits changed: " << average / 64 * 100 << "%";
    std::cout << " (" << average << " bits)" << std::endl;
    std::cout << "Standard deviation of bits changed: " << stdev << std::endl;
}

int main() {
    for (int i = 0; i < 10; ++i) {
        uint64_t P = math_utils::find_prime();
        std::vector<uint64_t> factors = math_utils::factorize_source(P);
        uint64_t G = math_utils::find_generator(factors, P);

        std::cout << "Testing with P = " << P << ", G = " << G << std::endl;

        run_avalanche_tests(P, G);
    }

    return 0;
}
