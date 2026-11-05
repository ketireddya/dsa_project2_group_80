#include "Benchmark.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "MergeSort.h"
#include "QuickSort.h"


// Checks every adjacent pair against the shared ranking rule.
bool isRanked(const std::vector<Title>& titles) {
    for (size_t i = 1; i < titles.size(); ++i) {
        if (ranksBefore(titles[i], titles[i - 1])) {
            return false;
        }
    }
    return true;
}

// compares every ranking with each sorting algorithm
bool sameRanking(const std::vector<Title>& a, const std::vector<Title>& b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].tconst != b[i].tconst) {
            return false;
        }
    }
    return true;
}

// Returns the middle of the three values, which is the median of three pivot selection strategy.
double medianOfThree(double a, double b, double c) {
    return a + b + c - std::min({a, b, c}) - std::max({a, b, c});
}

void runBenchmarks(const std::vector<Title>& allTitles) {
    if (allTitles.size() < 1000000) {
        std::cout << "\nBenchmarks require at least 1,000,000 loaded titles.\n";
        return;
    }

    std::error_code directoryError;
    std::filesystem::create_directories("benchmarks", directoryError);

    if (directoryError) {
        std::cout << "\nCould not create the benchmarks folder: "
                  << directoryError.message() << "\n";
        return;
    }

    std::ofstream csv("benchmarks/results.csv");

    if (!csv) {
        std::cout << "\nCould not open benchmarks/results.csv for writing.\n";
        return;
    }

    csv << "titles,input_order,run,merge_ms,quick_ms,merge_sorted,quick_sorted,rankings_match\n";

    const size_t sizes[] = {
        10000, 100000, 500000, allTitles.size()
    };

    const std::string orders[] = {
        "Random", "Sorted", "Reverse sorted"
    };

    std::cout << "\n========================================\n";
    std::cout << "           SORTING BENCHMARKS\n";
    std::cout << "========================================\n";
    std::cout << "\nCompares Quick Sort vs. Merge Sort \nacross different quantities of titles \nand ranking accuracy\n";
    std::cout << "Dataset: " << allTitles.size() << " rated titles\n";
    std::cout << "Quick Sort Pivot: Median of 3\n";
    std::cout << "Trials: 3 per input order - random, \nsorted, and reverse sorted\n";
    std::cout << "Current movie filters do not apply.\n";
    std::cout << "Timing excludes sorting and correctness \nchecks.\n";
    std::cout << "\nPress Enter to start";
    std::string input;
    std::getline(std::cin, input);

    // Shuffle indices so sampling does not copy the entire dataset.
    std::vector<size_t> indexes(allTitles.size());

    for (size_t i = 0; i < indexes.size(); ++i) {
        indexes[i] = i;
    }

    std::mt19937 rng(42); //fixed seed for reproducibility
    std::shuffle(indexes.begin(), indexes.end(), rng);

    bool allPassed = true;

    std::cout << "\n"
              << std::left
              << std::setw(12) << "Titles"
              << std::setw(18) << "Input order"
              << std::setw(15) << "Merge (ms)"
              << std::setw(15) << "Quick (ms)"
              << "Checks\n";
    std::cout << std::string(68, '-') << "\n";

    for (size_t n : sizes) {
        std::vector<Title> base;
        base.reserve(n); //reserves a designated amount of space to avoid reallocations

        for (size_t i = 0; i < n; ++i) {
            base.push_back(allTitles[indexes[i]]);
        }

        for (const std::string& order : orders) {
            // First experiment uses the random sample as-is.
            if (order == "Sorted") {
                mergeSort(base);
            }
            else if (order == "Reverse sorted") {
                std::reverse(base.begin(), base.end());
            }

            double mergeTimes[3] = {};
            double quickTimes[3] = {};
            bool experimentPassed = true;

            for (int run = 0; run < 3; ++run) {
                // Both algorithms receive identical, independent copies.
                std::vector<Title> merged = base;
                std::vector<Title> quick = base;

                auto timeMerge = [&]() {
                    auto start = std::chrono::steady_clock::now();
                    mergeSort(merged);
                    auto end = std::chrono::steady_clock::now();

                    mergeTimes[run] =
                        std::chrono::duration<double, std::milli>(
                            end - start
                        ).count();
                };

                auto timeQuick = [&]() {
                    auto start = std::chrono::steady_clock::now();
                    quickSort(quick, Pivot::MedianOfThree);
                    auto end = std::chrono::steady_clock::now();

                    quickTimes[run] =
                        std::chrono::duration<double, std::milli>(
                            end - start
                        ).count();
                };

                timeMerge();
                timeQuick();

                bool mergeSorted = isRanked(merged);
                bool quickSorted = isRanked(quick);
                bool rankingsMatch = sameRanking(merged, quick);

                bool passed =
                    mergeSorted && quickSorted && rankingsMatch;

                experimentPassed = experimentPassed && passed;

                csv << n << "," << order << "," << run + 1 << ","
                    << std::fixed << std::setprecision(6)
                    << mergeTimes[run] << "," << quickTimes[run] << ","
                    << mergeSorted << "," << quickSorted << ","
                    << rankingsMatch << "\n";

            }

            double mergeMedian = medianOfThree(
                mergeTimes[0], mergeTimes[1], mergeTimes[2]
            );

            double quickMedian = medianOfThree(
                quickTimes[0], quickTimes[1], quickTimes[2]
            );

            std::cout << std::left
                      << std::setw(12) << n
                      << std::setw(18) << order
                      << std::fixed << std::setprecision(3)
                      << std::setw(15) << mergeMedian
                      << std::setw(15) << quickMedian
                      << (experimentPassed ? "PASS" : "FAIL")
                      << "\n";

            allPassed = allPassed && experimentPassed;
        }
    }

    bool saved = static_cast<bool>(csv);
    csv.close();
    saved = saved && !csv.fail();

    std::cout << "\nResults: "
              << (allPassed ? "All 36 comparisons passed."
                            : "One or more comparisons FAILED.")
              << "\n";

    std::cout << "\nMemory Usage:\n";
    std::cout << "Merge Sort: O(n) - creates a temporary vector "
                 "the size of the list of movies\n";
    std::cout << "Quick Sort: O(log n) - uses a call stack "
                 "that grows logarithmically with the list size\n";

    if (saved) {
        std::cout << "\nResults saved to benchmarks/results.csv\n";
    }
    else {
        std::cout << "\nCould not save all benchmark results.\n";
    }

    std::cout << "\nPress Enter to return to the menu";
    std::getline(std::cin, input);
}