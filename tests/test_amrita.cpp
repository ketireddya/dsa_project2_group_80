#include <iostream>
#include "../src/DataLoader.h"

int main(int argc, char* argv[]) {
    // Default to the sample files; pass two paths to use the full files.
    std::string basics  = argc >= 3 ? argv[1] : "data/sample_basics.tsv";
    std::string ratings = argc >= 3 ? argv[2] : "data/sample_ratings.tsv";

    std::vector<Title> all = loadTitles(basics, ratings);
    std::cout << "Loaded " << all.size() << " rated titles\n";

    for (size_t i = 0; i < all.size() && i < 5; ++i) {
        const Title& t = all[i];
        std::cout << t.tconst << " | " << t.name << " | " << t.type
                  << " | year " << t.year << " | " << t.runtime << " min | "
                  << t.rating << " | " << t.votes << " votes\n";
    }
}