#pragma once
#include <vector>
#include "Title.h"

// a min heap that holds the k best titles seen so far
// min here means the worst of the k sits on top, so its the first one kicked out

class TopKHeap {
public:
    explicit TopKHeap(int k) : k_(k) {}

    // offer a title, but it only gets in if the heap isnt full yet
    // or if it beats the worst title currently in the heap

    void offer(const Title& t);

    int size() const { return (int)heap_.size(); }

    // empties the heap and gives back the titles best-first
    std::vector<Title> takeSorted();

private:
    int k_;
    std::vector<Title> heap_;  // heap_[0] is the worst of the ones we're keeping

    // true if a should sit above b in the heap (a is the worse title)
    static bool worse(const Title& a, const Title& b) { return ranksBefore(b, a); }

    void siftUp(int i);
    void siftDown(int i);
};

// the k best titles from the list, best-first, same order as the first k of a full sort, but 
// O(nlogk) instead of O(nlogn), and the list itself isnt changed
std::vector<Title> topk(const std::vector<Title>& titles, int k);