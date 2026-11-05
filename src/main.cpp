#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "DataLoader.h"
#include "Filter.h"
#include "Title.h"
#include "Trie.h"
#include "UserInterface.h"
#include "Benchmark.h"

// Returns true if input file is readable
bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

// Loads data and controls the application.
int main(int argc, char* argv[]) {
    std::string basicsPath;
    std::string ratingsPath;

    // Uses command-line paths when provided.
    if (argc >= 3) {
        basicsPath = argv[1];
        ratingsPath = argv[2];
    }
    else {
        basicsPath = "data/title.basics.tsv";
        ratingsPath = "data/title.ratings.tsv";
    }

    // Stops when either file remains missing.
    if (!fileExists(basicsPath) || !fileExists(ratingsPath)) {
        std::cerr << "Could not find the IMDb data files.\n";
        std::cerr << "Basics path: " << basicsPath << "\n";
        std::cerr << "Ratings path: " << ratingsPath << "\n";
        std::cerr << "\nRun the program like this:\n";
        std::cerr << "movie_picker.exe path-to-basics path-to-ratings\n";
        return 1;
    }

    std::cout << "Loading IMDb data...\n";
    std::cout << "Basics: " << basicsPath << "\n";
    std::cout << "Ratings: " << ratingsPath << "\n";
    std::cout << "This may take a moment.\n\n";

    std::vector<Title> allTitles = loadTitles(basicsPath, ratingsPath);

    if (allTitles.empty()) {
        std::cerr << "No IMDb titles were loaded.\n";
        return 1;
    }

    // Calculates every title's weighted score.
    computeScores(allTitles, 1000);

    Trie titleTrie;
    titleTrie.build(allTitles);

    std::cout << "Successfully loaded " << allTitles.size() << " rated titles.\n";

    // Stores the current filter settings.
    FilterOptions filters;
    std::string titleSearch;
    bool keepRunning = true;

    // Repeats until the user exits.
    while (keepRunning) {
        printCurrentFilters(filters, titleSearch);
        displayMainMenu();

        int selection = askForInteger("Selection: ", 1, 10);

        if (selection == 1) {
            searchTitles(titleSearch);
        }
        else if (selection == 2) {
            filters.type = chooseTitleType();
        }
        else if (selection == 3) {
            filters.genreMask = chooseGenre();
        }
        else if (selection == 4) {
            chooseYearRange(filters);
        }
        else if (selection == 5) {
            chooseRuntimeRange(filters);
        }
        else if (selection == 6) {
            chooseMinimumVotes(filters);
        }
        else if (selection == 7) {
            keepRunning = findTopMovies(allTitles, filters, titleTrie, titleSearch);
        }
        else if (selection == 8) {
            resetFilters(filters, titleSearch);
        }
        else if (selection == 9) {
            runBenchmarks(allTitles);        }
        else {
            keepRunning = false;
        }
    }

    std::cout << "\nGoodbye!\n";
    return 0;
}