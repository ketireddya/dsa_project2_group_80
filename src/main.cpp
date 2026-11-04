#include <algorithm>
#include <chrono> //includes time functions
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "DataLoader.h"
#include "Filter.h"
#include "MergeSort.h"
#include "Title.h"

// Returns true if input file is readable
bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

// Reads and validates a whole number.
// Reads an integer within the allowed range.
int askForInteger(const std::string& prompt, int minimumAllowed, int maximumAllowed) {
    while (true) {
        std::cout << prompt;

        std::string input;
        std::getline(std::cin, input);

        try {
            size_t charactersRead = 0;
            int value = std::stoi(input, &charactersRead);

            // Rejects text following the number.
            if (charactersRead != input.size()) {
                std::cout << "Please enter a whole number.\n";
                continue;
            }

            // Rejects numbers outside the range.
            if (value < minimumAllowed or value > maximumAllowed) {
                std::cout << "Enter a number from " << minimumAllowed << " to " << maximumAllowed << ".\n";
                continue;
            }

            return value;
        }
        catch (...) {
            // Handles text and empty input.
            std::cout << "Please enter a valid whole number.\n";
        }
    }
}

// Makes title types readable.
std::string titleTypeToText(const std::string& type) {
    if (type == "movie") {
        return "Movie";
    }
    else if (type == "tvSeries") {
        return "TV Series";
    }
    else if (type == "tvMiniSeries") {
        return "TV Miniseries";
    }
    else if (type.empty()) {
        return "Any Type";
    }
    else {
        return type;
    }
}

// Converts genre bits into readable names.
// Converts genre bits into readable text.
std::string genreMaskToText(uint32_t mask) {
    if (mask == 0) {
        return "Any Genre";
    }

    std::string result;
    const std::vector<std::string>& names = genreNames();

    for (size_t bit = 0; bit < names.size() && bit < 32; bit++) {
        uint32_t currentBit = 1u << bit;

        if ((mask & currentBit) != 0) {
            if (!result.empty()) {
                result += ", ";
            }

            result += names[bit];
        }
    }

    if (result.empty()) {
        return "Unknown";
    }
    else {
        return result;
    }
}

// Lets users choose a title type.
// Lets users choose a title type.
std::string chooseTitleType() {
    std::cout << "\nChoose a title type\n";
    std::cout << "1. Movie\n";
    std::cout << "2. TV Series\n";
    std::cout << "3. TV Miniseries\n";
    std::cout << "4. Any Type\n";

    int selection = askForInteger("Selection: ", 1, 4);

    if (selection == 1) {
        return "movie";
    }
    else if (selection == 2) {
        return "tvSeries";
    }
    else if (selection == 3) {
        return "tvMiniSeries";
    }
    else {
        return "";
    }
}

// Lets users choose a genre.
uint32_t chooseGenre() {
    const std::vector<std::string>& names = genreNames();

    std::cout << "\nChoose a genre\n";
    std::cout << "0. Any Genre\n";

    for (size_t i = 0; i < names.size(); i++) {
        std::cout << i + 1 << ". " << names[i] << "\n";
    }

    int selection = askForInteger("Selection: ", 0, static_cast<int>(names.size()));

    if (selection == 0) {
        return 0;
    }
    else {
        size_t bitPosition = static_cast<size_t>(selection - 1);
        return 1u << bitPosition;
    }
}

// Sets optional release year limits.
// Sets optional release year limits.
void chooseYearRange(FilterOptions& filters) {
    while (true) {
        std::cout << "\nEnter -1 for no limit.\n";

        int minimumYear = askForInteger("Minimum year: ", -1, 2100);
        int maximumYear = askForInteger("Maximum year: ", -1, 2100);

        if (minimumYear != -1 && maximumYear != -1 && minimumYear > maximumYear) {
            std::cout << "Minimum year cannot exceed maximum year.\n";
            continue;
        }

        filters.minYear = minimumYear;
        filters.maxYear = maximumYear;
        return;
    }
}

// Sets minimum and maximum runtimes.
// Sets optional runtime limits.
void chooseRuntimeRange(FilterOptions& filters) {
    while (true) {
        std::cout << "\nEnter -1 for no limit.\n";

        int minimumRuntime = askForInteger("Minimum minutes: ", -1, 1000);
        int maximumRuntime = askForInteger("Maximum minutes: ", -1, 1000);

        if (minimumRuntime != -1 && maximumRuntime != -1 && minimumRuntime > maximumRuntime) {
            std::cout << "Minimum runtime cannot exceed maximum runtime.\n";
            continue;
        }

        filters.minRuntime = minimumRuntime;
        filters.maxRuntime = maximumRuntime;
        return;
    }
}

// Sets the minimum vote count.
void chooseMinimumVotes(FilterOptions& filters) {
    std::cout << "\nEnter -1 for no minimum.\n";

    filters.minVotes = askForInteger("Minimum votes: ", -1, 100000000);
}

// Restores every filter to its default.
void resetFilters(FilterOptions& filters) {
    filters = FilterOptions{};
    std::cout << "\nAll filters have been reset.\n";
}

// Displays the currently selected filters.
void printCurrentFilters(const FilterOptions& filters) {
    std::cout << "\nCurrent Filters\n";
    std::cout << "----------------------------------------\n";

    std::cout << "Title Type: " << titleTypeToText(filters.type) << "\n";
    std::cout << "Genre: " << genreMaskToText(filters.genreMask) << "\n";

    std::cout << "Year Range: ";

    if (filters.minYear == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.minYear;
    }

    std::cout << " to ";

    if (filters.maxYear == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.maxYear;
    }

    std::cout << "\nRuntime Range: ";

    if (filters.minRuntime == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.minRuntime;
    }

    std::cout << " to ";

    if (filters.maxRuntime == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.maxRuntime;
    }

    std::cout << " minutes\n";
    std::cout << "Minimum Votes: ";

    if (filters.minVotes <= 0) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.minVotes;
    }

    std::cout << "\n";
    std::cout << "----------------------------------------\n";
}

// Displays up to ten ranked titles.
void printTopResults(const std::vector<Title>& titles);

// Returns to the menu or quits.
bool askAfterResults();

// Filters, sorts, times, and displays titles.
bool findTopMovies(
    const std::vector<Title>& allTitles,
    const FilterOptions& filters
);

// Displays the main terminal menu.
void displayMainMenu();

// Loads data and controls the application.
int main(int argc, char* argv[]);