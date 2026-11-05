#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include "../src/DataLoader.h"
#include "../src/Filter.h"
#include "../src/MergeSort.h"
#include "../src/QuickSort.h"
#include "../src/TopK.h"
#include "../src/Trie.h"

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

// top-k has to give the same titles as the front of a full sort
void testTopK() {
    std::vector<Title> base = randomTitles(100000, 99);
    std::vector<Title> sorted = base;
    quickSort(sorted);
    for (int k : { 0, 1, 10, 100, 200000 }) {   // 200000 is bigger than the list on purpose
        std::vector<Title> top = topK(base, k);
        size_t expectSize = std::min((size_t)k, sorted.size());
        std::vector<Title> expect(sorted.begin(), sorted.begin() + expectSize);
        std::cout << "top-" << k << ": " << (sameOrder(top, expect) ? "PASS" : "FAIL") << "\n";
    }
}


// small trie so we know exactly what should come back
void testTrie() {
    std::vector<Title> t(5);
    t[0].name = "Star Wars";
    t[1].name = "Star Trek";
    t[2].name = "Stardust";
    t[3].name = "Hamlet";
    t[4].name = "hamlet";          // same name, different case
    Trie trie;
    trie.build(t);

    std::cout << "\"star\" -> " << trie.startsWith("star").size() << " (expect 3)\n";
    std::cout << "\"STAR W\" -> " << trie.startsWith("STAR W").size() << " (expect 1)\n";
    std::cout << "\"star\" capped at 2 -> " << trie.startsWith("star", 2).size() << " (expect 2)\n";
    std::cout << "\"zzz\" -> " << trie.startsWith("zzz").size() << " (expect 0)\n";
    std::cout << "\"\" -> " << trie.startsWith("").size() << " (expect 5)\n";
    std::cout << "exact \"Hamlet\" -> " << trie.exact("Hamlet").size() << " (expect 2)\n";
    std::cout << "exact \"Ham\" -> " << trie.exact("Ham").size() << " (expect 0)\n";
}


int main(int argc, char* argv[]) {
    std::cout << "--- quick sort ---\n";
    testQuickSort();
    std::cout << "\n--- top-k heap ---\n";
    testTopK();
    std::cout << "\n--- trie ---\n";
    testTrie();

    // test using sample files unless we pass in two other paths
    std::string basics  = argc >= 3 ? argv[1] : "data/sample_basics.tsv";
    std::string ratings = argc >= 3 ? argv[2] : "data/sample_ratings.tsv";
    std::vector<Title> all = loadTitles(basics, ratings);
    std::cout << "\nLoaded " << all.size() << " rated titles\n";
    if (all.empty()) return 0;
    computeScores(all, 1000);
    std::cout << std::fixed << std::setprecision(3);

    // every movie with 100+ votes, timed with each sort
    FilterOptions movieOpts;
    movieOpts.type = "movie";
    movieOpts.minVotes = 100;
    std::vector<Title> movies = filterTitles(all, movieOpts);

    std::vector<Title> merged = movies;
    auto t0 = std::chrono::steady_clock::now();
    mergeSort(merged);
    std::cout << "Sorted " << movies.size() << " titles with Merge sort in " << msSince(t0) << " ms\n";

    for (Pivot p : ALL_PIVOTS) {
        std::vector<Title> q = movies;
        t0 = std::chrono::steady_clock::now();
        int depth = quickSort(q, p);               // only the sort gets timed
        long long ms = msSince(t0);
        std::cout << "Sorted " << q.size() << " titles with Quick sort (" << pivotName(p)
                  << ") in " << ms << " ms, depth " << depth << " ("
                  << (sameOrder(q, merged) ? "matches merge sort" : "DOES NOT MATCH") << ")\n";
    }

    // top 10 with the heap vs the front of the full sort
    t0 = std::chrono::steady_clock::now();
    std::vector<Title> top = topK(movies, 10);
    long long topMs = msSince(t0);
    std::vector<Title> front(merged.begin(), merged.begin() + std::min<size_t>(10, merged.size()));
    std::cout << "\ntop 10 movies with the heap in " << topMs << " ms ("
              << (sameOrder(top, front) ? "matches full sort" : "DOES NOT MATCH") << "):\n";
    for (size_t i = 0; i < top.size(); ++i)
        std::cout << i + 1 << ". " << top[i].name << " (" << top[i].year << ") | "
                  << top[i].rating << " | " << top[i].votes << " votes | score "
                  << top[i].score << "\n";

    // trie over every loaded title, then a search that uses top-k to show the best matches
    Trie trie;
    t0 = std::chrono::steady_clock::now();
    trie.build(all);
    std::cout << "\nbuilt trie of " << trie.wordCount() << " titles (" << trie.nodeCount()
              << " nodes) in " << msSince(t0) << " ms\n";

    std::string prefix = "the dark";
    std::vector<int> hits = trie.startsWith(prefix);
    std::vector<Title> matches;
    for (int i : hits) matches.push_back(all[i]);
    std::vector<Title> best = topK(matches, 5);
    std::cout << "\"" << prefix << "\": " << hits.size() << " matches, best 5:\n";
    for (const Title& t : best)
        std::cout << "  " << t.name << " (" << t.year << ", " << t.type << ") | score "
                  << t.score << "\n";
}
