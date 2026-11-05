#include "TopK.h"
#include <algorithm>
#include <utility>

// moves heap_[i] up while it's worse than its parent
void TopKHeap::siftUp(int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (!worse(heap_[i], heap_[parent])) break;   // parent is already the worse one
        std::swap(heap_[i], heap_[parent]);
        i = parent;
    }
}

// moves heap_[i] down while it's better than either child
void TopKHeap::siftDown(int i) {
    int n = (int)heap_.size();
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int worst = i;
        if (left < n && worse(heap_[left], heap_[worst]))   worst = left;
        if (right < n && worse(heap_[right], heap_[worst])) worst = right;
        if (worst == i) break;                         // i is worse than both kids, done
        std::swap(heap_[i], heap_[worst]);
        i = worst;
    }
}


void TopKHeap::offer(const Title& t) {
    if (k_ <= 0) return;
    if ((int)heap_.size() < k_) {                      // not full yet, just push it
        heap_.push_back(t);
        siftUp((int)heap_.size() - 1);
    } else if (ranksBefore(t, heap_[0])) {             // beats the worst one, we're keeping
        // same as push it, then pop the smallest, but done in one step
        // overwrite the worst title on top and let it sink to where it belongs
        heap_[0] = t;
        siftDown(0);
    }       // otherwise it's not top-k material, skip it (no copy made)
}

std::vector<Title> TopKHeap::takeSorted() {
    std::vector<Title> out;
    out.reserve(heap_.size());
    while (!heap_.empty()) {
        out.push_back(std::move(heap_[0]));        // pop the worst
        heap_[0] = std::move(heap_.back());
        heap_.pop_back();
        if (!heap_.empty()) siftDown(0);
    }
    std::reverse(out.begin(), out.end());       // so flip it to get best-first
    return out;
}



std::vector<Title> topK(const std::vector<Title>& titles, int k) {
    TopKHeap h(k);
    for (const Title& t : titles) h.offer(t);
    return h.takeSorted();
}

