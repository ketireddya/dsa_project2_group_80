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

// gives back the middle of three trial times, so one weirdly slow run doesn't throw off the result
double medianOfThree(double a, double b, double c) {
    return a + b + c - std::min({a, b, c}) - std::max({a, b, c});
}

// milliseconds between two moments
double msBetween(std::chrono::steady_clock::time_point start,
                 std::chrono::steady_clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
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

    // the three ways quick sort can pick its pivot, all get tested
    const Pivot pivots[] = { Pivot::First, Pivot::Random, Pivot::MedianOfThree };
    const int pivotCount = 3;

    // first pivot on sorted or reverse sorted input is O(n^2), so it only runs on small lists there.
    // at 100,000 titles that's around 5 billion comparisons per run, which takes way too long
    const size_t firstPivotLimit = 10000;

    csv << "titles,input_order,run,merge_ms,quick_first_ms,quick_random_ms,"
           "quick_median3_ms,all_sorted,rankings_match\n";

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
    std::cout << "Quick Sort Pivots: First, Random, Median of 3\n";
    std::cout << "First pivot is skipped on sorted and \nreverse sorted lists over "
              << firstPivotLimit << " titles.\n";
    std::cout << "Trials: 3 per input order - random, \nsorted, and reverse sorted\n";
    std::cout << "Current movie filters do not apply.\n";
    std::cout << "Timing only measures the sort itself, \nnot copying or correctness checks.\n";
    std::cout << "This takes a few minutes.\n";
    std::cout << "\nPress Enter to start";
    std::string input;
    std::getline(std::cin, input);

    // shuffle positions instead of copying the whole dataset
    std::vector<size_t> indexes(allTitles.size());

    for (size_t i = 0; i < indexes.size(); ++i) {
        indexes[i] = i;
    }

    std::mt19937 rng(42); // fixed seed so every run uses the same random sample
    std::shuffle(indexes.begin(), indexes.end(), rng);

    bool allPassed = true;
    int runsChecked = 0;

    std::cout << "\n"
              << std::left
              << std::setw(10) << "Titles"
              << std::setw(16) << "Input order"
              << std::setw(11) << "Merge"
              << std::setw(11) << "Q-First"
              << std::setw(11) << "Q-Random"
              << std::setw(11) << "Q-Med3"
              << "Checks\n";
    std::cout << std::setw(10) << "" << std::setw(16) << ""
              << "(median ms of 3 runs)\n";
    std::cout << std::string(76, '-') << "\n";

    for (size_t n : sizes) {
        std::vector<Title> base;
        base.reserve(n); // make room up front so it doesn't keep growing

        for (size_t i = 0; i < n; ++i) {
            base.push_back(allTitles[indexes[i]]);
        }

        for (const std::string& order : orders) {
            // random uses the shuffled sample as is, then it gets sorted, then flipped
            if (order == "Sorted") {
                mergeSort(base);
            }
            else if (order == "Reverse sorted") {
                std::reverse(base.begin(), base.end());
            }

            bool skipFirst = (order != "Random" && n > firstPivotLimit);

            double mergeTimes[3] = {};
            double quickTimes[3][3] = {};   // [which pivot][which run], -1 means skipped
            bool experimentPassed = true;

            for (int run = 0; run < 3; ++run) {
                // merge sort gets its own fresh copy
                std::vector<Title> merged = base;
                auto start = std::chrono::steady_clock::now();
                mergeSort(merged);
                auto end = std::chrono::steady_clock::now();
                mergeTimes[run] = msBetween(start, end);

                bool allSorted = isRanked(merged);
                bool rankingsMatch = true;

                for (int p = 0; p < pivotCount; ++p) {
                    if (p == 0 && skipFirst) {
                        quickTimes[p][run] = -1;
                        continue;
                    }

                    // every pivot gets its own fresh copy too, so nobody sorts already sorted data by accident
                    std::vector<Title> quick = base;
                    start = std::chrono::steady_clock::now();
                    quickSort(quick, pivots[p]);
                    end = std::chrono::steady_clock::now();
                    quickTimes[p][run] = msBetween(start, end);

                    allSorted = allSorted && isRanked(quick);
                    rankingsMatch = rankingsMatch && sameRanking(merged, quick);
                }

                bool passed = allSorted && rankingsMatch;
                experimentPassed = experimentPassed && passed;
                ++runsChecked;

                csv << n << "," << order << "," << run + 1 << ","
                    << std::fixed << std::setprecision(6) << mergeTimes[run];
                for (int p = 0; p < pivotCount; ++p) {
                    csv << ",";
                    if (quickTimes[p][run] < 0) csv << "skipped";
                    else csv << quickTimes[p][run];
                }
                csv << "," << allSorted << "," << rankingsMatch << "\n";
            }

            std::cout << std::left << std::fixed << std::setprecision(3)
                      << std::setw(10) << n
                      << std::setw(16) << order
                      << std::setw(11) << medianOfThree(mergeTimes[0], mergeTimes[1], mergeTimes[2]);

            for (int p = 0; p < pivotCount; ++p) {
                if (p == 0 && skipFirst) {
                    std::cout << std::setw(11) << "skipped";
                }
                else {
                    std::cout << std::setw(11)
                              << medianOfThree(quickTimes[p][0], quickTimes[p][1], quickTimes[p][2]);
                }
            }

            std::cout << (experimentPassed ? "PASS" : "FAIL") << "\n";

            allPassed = allPassed && experimentPassed;
        }
    }

    bool saved = static_cast<bool>(csv);
    csv.close();
    saved = saved && !csv.fail();

    std::cout << "\nResults: ";
    if (allPassed) {
        std::cout << "All " << runsChecked << " runs passed: every sort was in order and matched merge sort.\n";
    }
    else {
        std::cout << "One or more runs FAILED.\n";
    }

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