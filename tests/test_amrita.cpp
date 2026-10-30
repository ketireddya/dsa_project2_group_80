#include <iomanip>
#include <iostream>
#include "../src/DataLoader.h"
#include "../src/Filter.h"
#include <chrono>
#include <random>
#include "../src/MergeSort.h"

// turns the genre number back into words like "Comedy,Drama" so we can read it
std::string genresToText(uint32_t mask) {
    std::string out;
    const std::vector<std::string>& names = genreNames();
    for (size_t bit = 0; bit < names.size(); ++bit) {
        if (mask & (1u << bit)) {              // is this genre's bit on?
            if (!out.empty()) out += ",";
            out += names[bit];
        }
    }
    return out.empty() ? "(none)" : out;
}

// true if nothing in the list should come before the title in front of it
bool isRanked(const std::vector<Title>& a) {
    for (size_t i = 1; i < a.size(); ++i)
        if (ranksBefore(a[i], a[i - 1])) return false;
    return true;
}

// makes a fake title with just what the comparison looks at
Title make(const std::string& id, double score, int votes) {
    Title t; t.tconst = id; t.score = score; t.votes = votes; return t;
}

// runs merge sort on the cases that usually break sorting code
void testMergeSort() {
    std::vector<std::vector<Title>> cases = {
        {},                                                          // empty
        { make("tt1", 5, 10) },                                      // one title
        { make("tt1", 5, 10), make("tt2", 7, 10) },                  // two titles
        { make("tt3", 6, 1), make("tt1", 6, 1), make("tt2", 6, 1) }, // same score and votes
        { make("tt1", 9, 1), make("tt2", 8, 1), make("tt3", 7, 1) }, // already in order
        { make("tt1", 1, 1), make("tt2", 2, 1), make("tt3", 3, 1) }, // backwards
    };
    for (size_t i = 0; i < cases.size(); ++i) {
        size_t before = cases[i].size();
        mergeSort(cases[i]);
        bool ok = isRanked(cases[i]) && cases[i].size() == before;   // sorted and nothing lost
        std::cout << "case " << i << ": " << (ok ? "PASS" : "FAIL") << "\n";
    }

    // 100k random fake titles. same seed every run so results don't change
    std::mt19937 rng(42);
    std::vector<Title> r;
    for (int i = 0; i < 100000; ++i)
        r.push_back(make("tt" + std::to_string(i), (rng() % 1000) / 100.0, rng() % 50));
    mergeSort(r);
    std::cout << "random 100k: " << (isRanked(r) && r.size() == 100000 ? "PASS" : "FAIL") << "\n";
}

int main(int argc, char* argv[]) {
    // use the sample files unless we pass in two other paths
    std::string basics  = argc >= 3 ? argv[1] : "data/sample_basics.tsv";
    std::string ratings = argc >= 3 ? argv[2] : "data/sample_ratings.tsv";

    std::vector<Title> all = loadTitles(basics, ratings);
    std::cout << "Loaded " << all.size() << " rated titles\n";

    std::cout << "Genres found (" << genreNames().size() << "):";
    for (const std::string& g : genreNames()) std::cout << " " << g;
    std::cout << "\n\n";

    const int m = 1000;
    computeScores(all, m);

    // works out the average again here so we can check a score by hand
    double sum = 0.0;
    for (const Title& t : all) sum += t.rating;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "m = " << m << ", average rating C = " << sum / all.size() << "\n\n";

    for (size_t i = 0; i < all.size() && i < 5; ++i) {
        const Title& t = all[i];
        std::cout << t.tconst << " | " << t.name << " | " << t.type
                  << " | year " << t.year << " | " << t.runtime << " min | "
                  << t.rating << " | " << t.votes << " votes | "
                  << genresToText(t.genreMask) << " | score " << t.score << "\n";
    }

    // the title with the most votes should have a score super close to its own rating
    size_t most = 0;
    for (size_t i = 1; i < all.size(); ++i)
        if (all[i].votes > all[most].votes) most = i;
    std::cout << "\nmost votes: " << all[most].name << " | " << all[most].rating
              << " | " << all[most].votes << " votes | score " << all[most].score << "\n";

    // no filters set should keep everything
    std::cout << "\nno filters keeps all: "
              << (filterTitles(all, FilterOptions{}).size() == all.size() ? "PASS" : "FAIL") << "\n";

    // try a real filter: comedy or horror movies from 2000-2010, 90+ min, 10k+ votes
    FilterOptions opts;
    opts.type = "movie";
    opts.genreMask = genreBit("Comedy") | genreBit("Horror");
    opts.minYear = 2000;
    opts.maxYear = 2010;
    opts.minRuntime = 90;
    opts.minVotes = 10000;
    std::vector<Title> picked = filterTitles(all, opts);
    std::cout << "filter kept " << picked.size() << " of " << all.size() << " titles\n";

    // double check every kept title actually passes, in case the filter has a bug
    int bad = 0;
    for (const Title& t : picked) {
        bool ok = t.type == "movie" && (t.genreMask & opts.genreMask) != 0
               && t.year >= 2000 && t.year <= 2010
               && t.runtime >= 90 && t.votes >= 10000;
        if (!ok) ++bad;
    }
    std::cout << "titles that shouldn't be there: " << bad << "\n";

    for (size_t i = 0; i < picked.size() && i < 5; ++i) {
        const Title& t = picked[i];
        std::cout << t.name << " (" << t.year << ") | " << t.runtime << " min | "
                  << t.votes << " votes | " << genresToText(t.genreMask) << "\n";
    }

    std::cout << "\n";
    testMergeSort();

    // sort the filtered movies from above and show the top 10
    mergeSort(picked);
    std::cout << "\ntop 10 comedy/horror movies 2000-2010 ("
              << (isRanked(picked) ? "order OK" : "ORDER WRONG") << "):\n";
    for (size_t i = 0; i < picked.size() && i < 10; ++i) {
        const Title& t = picked[i];
        std::cout << i + 1 << ". " << t.name << " (" << t.year << ") | " << t.rating
                  << " | " << t.votes << " votes | score " << t.score << "\n";
    }

    // time a bigger sort: every movie with 100+ votes
    FilterOptions movieOpts;
    movieOpts.type = "movie";
    movieOpts.minVotes = 100;
    std::vector<Title> movies = filterTitles(all, movieOpts);

    auto start = std::chrono::steady_clock::now();
    mergeSort(movies);                       // only the sort gets timed
    auto end = std::chrono::steady_clock::now();
    std::cout << "\nSorted " << movies.size() << " titles with Merge sort in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms (" << (isRanked(movies) ? "order OK" : "ORDER WRONG") << ")\n";
}