#pragma once
#include <vector>
#include "Title.h"

// the three ways quick sort can pick its pivot
enum class Pivot {
    First,  // always the first title in the range (simple, but slow on sorted input)
    Random, // a random title in the range
    MedianOfThree   // the middle of the first, middle, and last
};

// sorts the list best-first using ranksbefore, same order as merge sort
// returns the deepest the recursion went, can talk about it in report
int quickSort(std::vector<Title>& a, Pivot p = Pivot::MedianOfThree);

// "first", "Random", "Median of 3" for the menu and output
const char* pivotName(Pivot p);
