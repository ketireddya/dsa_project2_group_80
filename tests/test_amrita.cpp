#include <iomanip>
#include <iostream>
#include "../src/DataLoader.h"

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
}