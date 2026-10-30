#include "MergeSort.h"
#include <utility>

// takes two halves that are already sorted, a[lo..mid] and a[mid+1..hi],
// and zips them together into one sorted run. tmp is scratch space
static void merge(std::vector<Title>& a, std::vector<Title>& tmp,
                  int lo, int mid, int hi) {
    int i = lo;        // next spot to look at in the left half
    int j = mid + 1;   // next spot to look at in the right half
    int k = lo;        // next open spot in tmp

    while (i <= mid && j <= hi) {
        // only take from the right if it's actually better.
        // on a tie the left one goes first, which keeps equal titles in their original order
        if (ranksBefore(a[j], a[i])) tmp[k++] = std::move(a[j++]);
        else                         tmp[k++] = std::move(a[i++]);
    }
    while (i <= mid) tmp[k++] = std::move(a[i++]);   // whatever's left in the left half
    while (j <= hi)  tmp[k++] = std::move(a[j++]);   // whatever's left in the right half

    for (int x = lo; x <= hi; ++x) a[x] = std::move(tmp[x]);  // put it all back
}

static void mergeSortRec(std::vector<Title>& a, std::vector<Title>& tmp,
                         int lo, int hi) {
    if (lo >= hi) return;                  // 0 or 1 titles is already sorted
    int mid = lo + (hi - lo) / 2;          // middle, written this way so it can't overflow
    mergeSortRec(a, tmp, lo, mid);         // sort the left half
    mergeSortRec(a, tmp, mid + 1, hi);     // sort the right half
    merge(a, tmp, lo, mid, hi);            // zip them together
}

void mergeSort(std::vector<Title>& a) {
    if (a.size() < 2) return;
    std::vector<Title> tmp(a.size());      // the extra space, made once up front
    mergeSortRec(a, tmp, 0, (int)a.size() - 1);
}