#include <iostream>
#include "../src/DataLoader.h"

// turns a genre bitmask back into text like "Comedy,Drama" for printing.
std::string genresToText(uint32_t mask) {
    std::string out;
    const std::vector<std::string>& names = genreNames();
    for (size_t bit = 0; bit < names.size(); ++bit) {
        if (mask & (1u << bit)) {
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

    for (size_t i = 0; i < all.size() && i < 5; ++i) {
        const Title& t = all[i];
        std::cout << t.tconst << " | " << t.name << " | " << t.type
                  << " | year " << t.year << " | " << t.runtime << " min | "
                  << t.rating << " | " << t.votes << " votes | "
                  << genresToText(t.genreMask) << "\n";
    }
}