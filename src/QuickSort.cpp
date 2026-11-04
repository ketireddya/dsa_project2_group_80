#include "QuickSort.h"
#include <random>
#include <utility>

namespace { 
    
std::mt19937 g_rng(12345); // fixed seed so random pivot picks the same way every run

// picks which spot in a[lo..hi] to use as the pivot
int choosePivot(const std::vector<Title>& a, int lo, int hi, Pivot p) {
    if (p == Pivot::First) return lo;
    if (p == Pivot::Random) {
        std::uniform_int_distribution<int> pick(lo, hi);
        return pick(g_rng);
    }
    
// median of three
int mid = lo + (hi - lo) / 2;
    const Title& x = a[lo];
    const Title& y = a[mid];
    const Title& z = a[hi];
    if (ranksBefore(x, y)) {
        if (ranksBefore(y, z)) return mid;   // x, y, z
        if (ranksBefore(x, z)) return hi;    // x, z, y
        return lo;                           // z, x, y
    } else {
        if (ranksBefore(x, z)) return lo;    // y, x, z
        if (ranksBefore(y, z)) return hi;    // y, z, x
        return mid;                          // z, y, x
    }
}


// the pivot has already been moved to a[hi]
// everything that ranks before the pivot goes to the left, everything that ranks after goes to the right
int partition(std::vector<Title>& a, int lo, int hi) {
    int i = lo;                             //next open spot on the better than pivot side
    for (int j = lo; j < hi; ++j) {
        if (ranksBefore(a[j], a[hi])) {
            std:: swap(a[i], a[j]);
            ++i;
        }
    }
    std::swap(a[i], a[hi]);         // pivot goes right after the better ones
    return i;
}   

void quickSortRec(std::vector<Title>& a, int lo, int hi, Pivot p, int depth, int& maxDepth) {
    if (depth > maxDepth) maxDepth = depth;

// if we only recurse on the smaller side and loop on the bigger side that keeps the recursion
// at most log2(n) deep, so even a bad pivot on 1 million already sorted titles cant overflow
// the stack (instead it's just slow)
    while (lo < hi) {
        int pi = choosePivot(a, lo, hi, p);
        std::swap(a[pi], a[hi]);    // move the pivot to the end
        int k = partition(a, lo, hi);  // a[k] is now in its final spot

        if (k - lo < hi - k) {
            quickSortRec(a, lo, k - 1, p, depth + 1, maxDepth);     //left is smaller
            lo = k + 1;                                 // keep going on the right
        } else {
            quickSortRec(a, k+1, hi, p, depth + 1, maxDepth);    // right is smaller
            hi = k - 1;                                 // keep going on the left
        }
    }
}
} // end namespace

int quickSort(std::vector<Title>& a, Pivot p) {
    int maxDepth = 0;
    if (a.size() < 2) return maxDepth;         // 0 or 1 titles is already sorted
    quickSortRec(a, 0, (int)a.size() - 1, p, 1, maxDepth);
    return maxDepth;
}

const char* pivotName(Pivot p) {
    switch (p) {
        case Pivot::First:  return "First";
        case Pivot::Random: return "Random";
        default:            return "Median of 3";
    }
}
