#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include "../src/MergeSort.h"
#include "../src/QuickSort.h"
#include "../src/TopK.h"


const Pivot ALL_PIVOTS[] = { Pivot::First, Pivot::Random, Pivot::MedianOfThree };

// true if nothing in the list should come before the title in front of it
bool isRanked(const std::vector<Title>& a) {
    for (size_t i = 1; i < a.size(); ++i)
        if (ranksBefore(a[i], a[i - 1])) return false;
    return true;
}

// true if both lists have the exact same titles in the exact same order
bool sameOrder(const std::vector<Title>& a, const std::vector<Title>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i].tconst != b[i].tconst) return false;
    return true;
}

// makes a fake title with just what the comparison looks at
Title make(const std::string& id, double score, int votes) {
    Title t; t.tconst = id; t.score = score; t.votes = votes; return t;
}

// n random fake titles, same seed every run so results don't change
std::vector<Title> randomTitles(int n, unsigned seed) {
    std::mt19937 rng(seed);
    std::vector<Title> r;
    r.reserve(n);
    for (int i = 0; i < n; ++i)
        r.push_back(make("tt" + std::to_string(i), (rng() % 1000) / 100.0, rng() % 50));
    return r;
}

long long msSince(std::chrono::steady_clock::time_point start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
}



// runs every pivot on the cases that usually break sorting code
void testQuickSort() {
    std::vector<std::vector<Title>> cases = {
        {},                                                          // empty
        { make("tt1", 5, 10) },                                      // one title
        { make("tt1", 5, 10), make("tt2", 7, 10) },                  // two titles
        { make("tt3", 6, 1), make("tt1", 6, 1), make("tt2", 6, 1) }, // same score and votes
        { make("tt1", 9, 1), make("tt2", 8, 1), make("tt3", 7, 1) }, // already in order
        { make("tt1", 1, 1), make("tt2", 2, 1), make("tt3", 3, 1) }, // backwards
    };
    for (Pivot p : ALL_PIVOTS) {
        std::cout << "pivot " << pivotName(p) << ":";
        for (size_t i = 0; i < cases.size(); ++i) {
            std::vector<Title> c = cases[i];
            std::vector<Title> expect = cases[i];
            quickSort(c, p);
            mergeSort(expect);
            bool ok = isRanked(c) && sameOrder(c, expect);   // sorted, nothing lost, matches merge
            std::cout << " case" << i << "=" << (ok ? "PASS" : "FAIL");
        }
        std::cout << "\n";
    }

    // 100k random, every pivot has to match merge sort exactly
    std::vector<Title> base = randomTitles(100000, 42);
    std::vector<Title> merged = base;
    mergeSort(merged);
    for (Pivot p : ALL_PIVOTS) {
        std::vector<Title> q = base;
        int depth = quickSort(q, p);
        std::cout << "random 100k, " << pivotName(p) << ": "
                  << (sameOrder(q, merged) ? "PASS" : "FAIL")
                  << " (max recursion depth " << depth << ")\n";
    }

    // sorted and backwards input, this is where the First pivot falls apart
    // so you have to keep n small or the test will take forever
     const int n = 20000;
    std::vector<Title> sorted = randomTitles(n, 7);
    mergeSort(sorted);
    std::vector<Title> backwards(sorted.rbegin(), sorted.rend());
    for (Pivot p : ALL_PIVOTS) {
        std::vector<Title> s = sorted, b = backwards;
        auto t0 = std::chrono::steady_clock::now();
        quickSort(s, p);
        long long sMs = msSince(t0);
        t0 = std::chrono::steady_clock::now();
        quickSort(b, p);
        long long bMs = msSince(t0);
        std::cout << pivotName(p) << " on " << n << " sorted: " << sMs << " ms ("
                  << (sameOrder(s, sorted) ? "PASS" : "FAIL") << "), backwards: " << bMs
                  << " ms (" << (sameOrder(b, sorted) ? "PASS" : "FAIL") << ")\n";
    }
}

int main() {
    std::cout << "--- quick sort ---\n";
    testQuickSort();
}